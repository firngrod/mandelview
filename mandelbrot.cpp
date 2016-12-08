#include <iostream>
#include <gmpxx.h>
#include <list>
#include <mutex>
#include <thread>
#include "workerthread.hpp"
#include "json/json.h"
#include "CyclicVar.hpp"

extern int precision;

#include "mandelbrot.hpp"
namespace Mandelbrot
{
  bool CountIterations(uint64_t &iterations, const Complex &point, const uint64_t &maxIterations, mpf_class &tmpBuf)
  {
    Complex buf[2];
    FirnLibs::CyclicVar<Complex *> pos(buf + 1, buf, buf); // Cycle through the points.
    iterations = 0;
    bool divergent = false;
    while(!divergent && iterations < maxIterations)
    {
      divergent = Complex::Square(*(++pos), *(pos), tmpBuf);
      *(pos) += point;
      ++iterations;
    }

    return divergent;
  }

  bool ProvenDivergent(const Complex &p, mpf_class &tmpBuf)
  {
    if(2 <= p.r || 2 <= p.i)
      return true;
    tmpBuf = p.r * p.r + p.i * p.i;
    if(tmpBuf >= 4)
      return true;

    return false;
  }

  void CalculateView(MandelbrotView &viewOut, const Json::Value &viewDefs)
  {
    viewOut = MandelbrotView();

    viewOut.maxItr = viewDefs.get("MaxIterations", 255).asUInt64();
    viewOut.numThreads = viewDefs.get("NumThreads", 8).asInt();
    if(!viewOut.numThreads)
      viewOut.numThreads = 1;

    viewOut.imDimX = viewDefs.get("OutputSize", Json::Value()).get("X", 1920).asInt();
    viewOut.imDimY = viewDefs.get("OutputSize", Json::Value()).get("Y", 1200).asInt();
    bool xIsLargest = viewOut.imDimX > viewOut.imDimY;

    viewOut.span = viewDefs.get("Span", "3").asString();
    mpf_class forPrecisionEstimate(viewOut.span);
    forPrecisionEstimate /= xIsLargest ? viewOut.imDimX : viewOut.imDimY;
    mp_exp_t exp;
    forPrecisionEstimate.get_str(exp, 2);
    precision = abs(exp) + 40;

    viewOut.centerX = viewDefs.get("Center", Json::Value()).get("Real", "-0.5").asString();
    viewOut.centerY = viewDefs.get("Center", Json::Value()).get("Imaginary", "0.0").asString();


    viewOut.fitting = viewDefs.get("Fitting", "Fit").asString();
    mpf_class dx(0, precision), dy(0, precision), startX(0, precision), startY(0, precision);
    if(viewOut.fitting == "Stretch")
    {
      dx = viewOut.span / viewOut.imDimX;
      dy = viewOut.span / viewOut.imDimY;
      startX = viewOut.centerX - viewOut.span/2;
      startY = viewOut.centerY + viewOut.span/2;
    }
    else if(viewOut.fitting == "Fit")
    {
      int smallestDim = xIsLargest ? viewOut.imDimY : viewOut.imDimX;
      dx = dy = viewOut.span / smallestDim;
      startX = viewOut.centerX - dx * viewOut.imDimX / 2;
      startY = viewOut.centerY + dy * viewOut.imDimY / 2;
    }
    viewOut.spanX = dx * viewOut.imDimX;
    viewOut.spanY = dy * viewOut.imDimY;

    Complex startPoint(startX, startY);

    // Ok, this is our next project:  Fun with passes.
    // The theory is that the Mandelbrot Set is connected.  Therefore, every layer of iterations is connected.
    // Therefore, if one can see that every point on a closed curve is in the same layer, all points within the closed curve is within the layer.
    // Except of course if the closed curve encompasses the entire Mandelbrot Set.
    // What we do is this:  We split the image into squares of pixel size 2^passes to a side.  For the first pass, we calculate a point for each square.
    // Then we do another pass in which we look at the previous pass and make a gross assumption:
    // If the nearest four points all have the same number of iterations, we assume that the entire square border has the same amount of iterations.
    // This is obviously not always the case.  The entire Mandelbrot Set can be within the square, or any number of iteration count borders can have
    // entered and left the square along the edge, such as if a narrow spike goes through the square.  Therefore, this method should NOT be used
    // for the final rendition, only for speed increases while searching.
    // Continues until every pixel is rendered
    viewOut.passes = viewDefs.get("Passes", 4).asInt();
    // Pad the image such that the image fits the squares.  Also add squares at the right hand side and bottom for references for the pixels near then.
    viewOut.paddedDimX = ((viewOut.imDimX >> viewOut.passes) + 2) << viewOut.passes;
    viewOut.paddedDimY = ((viewOut.imDimY >> viewOut.passes) + 2) << viewOut.passes;
    // Zeroinitialize.
    viewOut.data.resize(viewOut.paddedDimX * viewOut.paddedDimY, 0);
    uint64_t * dataGrid = &viewOut.data[0];
    CalculatorParams::paddedDimX = viewOut.paddedDimX;
    std::list<std::vector<CalculatorParams *> *> workerThreadJobs;
    std::mutex dataMutex;
    CalculatorParams::totalPoints = 0;
    CalculatorParams::pointsDone = 0;
    for(int passesLeft = viewOut.passes; passesLeft > 0; passesLeft--)
    {
      Complex currentPoint(startX, startY);
      uint64_t * dataPtr = dataGrid;
      int stepSize = 1 << (passesLeft - 1);
      bool isFirstPass = viewOut.passes == passesLeft;

      CalculatorParams::maxItr = viewOut.maxItr;
      
      // We iterate through to the far side the first time, but not subsequently.  If we did, we would go out of bounds on our probes
      for(int yItr = 0; yItr < viewOut.paddedDimY - (isFirstPass ? 1 : (1 << viewOut.passes)); yItr += stepSize)
      {
        std::vector<CalculatorParams *> *lineVector = new std::vector<CalculatorParams *>();
        lineVector->reserve(viewOut.paddedDimX >> passesLeft);
        for(int xItr = 0; xItr < viewOut.paddedDimX - (isFirstPass ? 0 : (1 << viewOut.passes)); xItr += stepSize)
        {
          lineVector->push_back(new CalculatorParams(currentPoint, dataPtr, !isFirstPass ? stepSize : 0));
          dataPtr += stepSize;
          currentPoint.r += dx * stepSize;
        }
        workerThreadJobs.push_back(lineVector);
        CalculatorParams::totalPoints += lineVector->size();
        currentPoint.i -= dy * stepSize;
        dataPtr += viewOut.paddedDimX * (stepSize - 1) + (isFirstPass ? 0 : (1 << viewOut.passes));
        currentPoint.r = startPoint.r;
      }
    }
    std::vector<std::thread> workerThreads;
    for(int i = 0; i < viewOut.numThreads; i++)
    {
      WorkerParams * thisThreadParams = new WorkerParams();
      thisThreadParams->dataMutex = &dataMutex;
      thisThreadParams->workerThreadJobs = &workerThreadJobs;
      workerThreads.push_back(std::thread(WorkerThread, thisThreadParams));
    }

    for(std::vector<std::thread>::iterator threadItr = workerThreads.begin(), threadEnd = workerThreads.end(); threadItr != threadEnd; threadItr++)
    {
      threadItr->join();
    }
  }
}


