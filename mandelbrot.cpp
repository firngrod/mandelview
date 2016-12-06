#include <iostream>
#include <gmpxx.h>
#include <list>
#include <mutex>
#include <thread>
#include "workerthread.hpp"
#include "json/json.h"
#include "mandelbrot.hpp"
#include "CyclicVar.hpp"

extern int precision;

namespace Mandelbrot
{
  bool CountIterations(uint64_t &iterations, const Complex &point, const uint64_t &maxIterations, mpf_class &tmpBuf)
    //mpf_class &r, mpf_class &i, mpf_class &rSqr, mpf_class &iSqr, mpf_class &summie)
  {
    //r = 0;
    //i = 0;
    //rSqr = 0;
    //iSqr = 0;

    
    //bool divergent = (point.i * point.i + point.r * point.r) > 4.0;

    //while(!divergent)
    //{
      //summie = r + i;
      //mpf_pow_ui(i.__get_mp(), summie.__get_mp(), 2);
      //i += point.i - rSqr - iSqr;
      //r = rSqr - iSqr + point.r;
      //mpf_pow_ui(rSqr.__get_mp(), r.__get_mp(), 2);
      //mpf_pow_ui(iSqr.__get_mp(), i.__get_mp(), 2);
      //summie = rSqr + iSqr;
      //divergent = 4.0 < summie;
      //iterations++;



      //summie = r;
      //summie += i;
      //mpf_pow_ui(summie.__get_mp(), summie.__get_mp(), 2);
      //i = summie;
      //i -= rSqr;
      //i -= iSqr;
      //i += point.i;
      //r = rSqr;
      //r -= iSqr;
      //r += point.r;
      //mpf_pow_ui(rSqr.__get_mp(), r.__get_mp(), 2);
      //mpf_pow_ui(iSqr.__get_mp(), i.__get_mp(), 2);
      //summie = rSqr;
      //summie += iSqr;
      //divergent = 4.0 < summie;
      //iterations++;

    //}

    //return divergent;

    Complex buf[2];
    //Complex prevPts[4];
    //FirnLibs::CyclicVar<Complex *> prevs(prevPts + 3, prevPts, prevPts);
    FirnLibs::CyclicVar<Complex *> pos(buf + 1, buf, buf); // Cycle through the points.
    iterations = 0;
    bool divergent = false;
    while(!divergent && iterations < maxIterations)
    {
      divergent = Complex::Square(*(++pos), *(pos), tmpBuf);
      *(pos) += point;
      //if(*(++pos) == *(++pos))
      //{
        ////std::cout << "Cyclic detected, abandoning.\n";
        //iterations = maxIterations;
      //}
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
    uint64_t maxIterations = viewDefs.get("MaxIterations", 255).asUInt64();
    int numThreads = viewDefs.get("NumThreads", 8).asInt();
    if(!numThreads)
      numThreads = 1;

    int imDimX = viewDefs.get("OutputSize", Json::Value()).get("X", 800).asInt();
    int imDimY = viewDefs.get("OutputSize", Json::Value()).get("Y", 800).asInt();
    viewOut.imDimX = imDimX;
    viewOut.imDimY = imDimY;
    bool xIsLargest = imDimX > imDimY;

    mpf_class span(viewDefs.get("Span", "3").asString());
    mpf_class forPrecisionEstimate(span);
    forPrecisionEstimate /= xIsLargest ? imDimX : imDimY;
    mp_exp_t exp;
    forPrecisionEstimate.get_str(exp, 2);
    precision = abs(exp) + 40;

    mpf_class centerX(viewDefs.get("Center", Json::Value()).get("Real", "-0.5").asString(), precision);
    mpf_class centerY(viewDefs.get("Center", Json::Value()).get("Imaginary", "0.0").asString(), precision);


    std::string fitting = viewDefs.get("Fitting", "Fit").asString();
    mpf_class dx(0, precision), dy(0, precision), startX(0, precision), startY(0, precision);
    if(fitting == "Stretch")
    {
      dx = span / imDimX;
      dy = span / imDimY;
      startX = centerX - span/2;
      startY = centerY + span/2;
    }
    else if(fitting == "Fit")
    {
      int smallestDim = xIsLargest ? imDimY : imDimX;
      dx = dy = span / smallestDim;
      startX = centerX - dx * imDimX / 2;
      startY = centerY + dy * imDimY / 2;
    }

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
    int passes = viewDefs.get("Passes", 4).asInt();
    // Pad the image such that the image fits the squares.  Also add squares at the right hand side and bottom for references for the pixels near then.
    int paddedSizeX = ((imDimX >> passes) + 2) << passes;
    std::cout << paddedSizeX << " " << imDimX << std::endl;
    int paddedSizeY = ((imDimY >> passes) + 2) << passes;
    viewOut.paddedDimX = paddedSizeX;
    viewOut.paddedDimY = paddedSizeY;
    viewOut.maxItr = maxIterations;
    // Zeroinitialize.
    viewOut.data.resize(paddedSizeX * paddedSizeY, 0);
    uint64_t * dataGrid = &viewOut.data[0];
    CalculatorParams::paddedDimX = paddedSizeX;
    std::list<std::vector<CalculatorParams *> *> workerThreadJobs;
    std::mutex dataMutex;
    CalculatorParams::totalPoints = 0;
    CalculatorParams::pointsDone = 0;
    for(int passesLeft = passes; passesLeft > 0; passesLeft--)
    {
      Complex currentPoint(startX, startY);
      uint64_t * dataPtr = dataGrid;
      int stepSize = 1 << (passesLeft - 1);
      bool isFirstPass = passes == passesLeft;

      CalculatorParams::maxItr = maxIterations;
      
      // We iterate through to the far side the first time, but not subsequently.  If we did, we would go out of bounds on our probes
      for(int yItr = 0; yItr < paddedSizeY - (isFirstPass ? 1 : (1 << passes)); yItr += stepSize)
      {
        std::vector<CalculatorParams *> *lineVector = new std::vector<CalculatorParams *>();
        lineVector->reserve(paddedSizeX >> passesLeft);
        for(int xItr = 0; xItr < paddedSizeX - (isFirstPass ? 0 : (1 << passes)); xItr += stepSize)
        {
          lineVector->push_back(new CalculatorParams(currentPoint, dataPtr, !isFirstPass ? stepSize : 0));
          dataPtr += stepSize;
          currentPoint.r += dx * stepSize;
        }
        workerThreadJobs.push_back(lineVector);
        CalculatorParams::totalPoints += lineVector->size();
        currentPoint.i -= dy * stepSize;
        dataPtr += paddedSizeX * (stepSize - 1) + (isFirstPass ? 0 : (1 << passes));
        currentPoint.r = startPoint.r;
      }
    }
    std::vector<std::thread> workerThreads;
    for(int i = 0; i < numThreads; i++)
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


