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
    FirnLibs::CyclicVar<Complex *> pos(buf + 1, buf, buf); // Cycle through the points.
    iterations = 0;
    bool divergent = false;
    while(iterations < maxIterations)
    {
      //std::cout << "Starting point: " << point.r << " + " << point.i << "i\n";
      divergent = Complex::Square(*(++pos), *(pos), tmpBuf);
      if(divergent)
        break;
      //std::cout << "After multiplication " << (*pos).r << " + " << (*pos).i << "i" << std::endl;
      *(pos) += point;
      //std::cout << "After addition " << (*pos).r << " + " << (*pos).i << "i" << std::endl;
      ++iterations;
      //divergent = ProvenDivergent(*pos, tmpBuf);
      //std::cout << "Iteration: " << iterations << std::endl;
      //std::cout << (*pos).r << " + " << (*pos).i << "i" << std::endl;
      //std::cout << (Complex*)pos << std::endl;
    }

    //std::cout << "Ran iterations: " << iterations << std::endl;
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
    int numThreads = viewDefs.get("NumThreads", 1).asInt();
    if(!numThreads)
      numThreads = 1;

    int imDimX = viewDefs.get("OutputSize", Json::Value()).get("x", 800).asInt();
    int imDimY = viewDefs.get("OutputSize", Json::Value()).get("y", 800).asInt();
    viewOut.imDimX = imDimX;
    viewOut.imDimY = imDimY;
    bool xIsLargest = imDimX > imDimY;

    mpf_class zoom(viewDefs.get("Zoom", "4").asString());
    mpf_class forPrecisionEstimate(zoom);
    forPrecisionEstimate /= xIsLargest ? imDimX : imDimY;
    mp_exp_t exp;
    forPrecisionEstimate.get_str(exp, 2);
    precision = abs(exp) + 40;

    mpf_class centerX(viewDefs.get("Center", Json::Value()).get("Real", "0").asString(), precision);
    mpf_class centerY(viewDefs.get("Center", Json::Value()).get("Imaginary", "0").asString(), precision);


    std::string fitting = viewDefs.get("Fitting", "Fit").asString();
    mpf_class dx(0, precision), dy(0, precision), startX(0, precision), startY(0, precision);
    if(fitting == "Stretch")
    {
      dx = zoom / imDimX;
      dy = zoom / imDimY;
      startX = centerX - zoom/2;
      startY = centerY + zoom/2;
    }
    else if(fitting == "Fit")
    {
      int smallestDim = xIsLargest ? imDimY : imDimX;
      dx = dy = zoom / smallestDim;
      startX = centerX - dx * imDimX / 2;
      startY = centerY + dy * imDimY / 2;
    }

    Complex startPoint(startX, startY);
    Complex currentPoint(startX, startY);
                        
    viewOut.data.resize(imDimX * imDimY);
    uint64_t * dataGrid = &viewOut.data[0];
    uint64_t * dataPtr = dataGrid;
    --dataPtr;

    mpf_class tmpBuffer(0, precision);

    std::list<std::vector<CalculatorParams *> *> workerThreadJobs;
    std::mutex dataMutex;
    
    for(int yItr = 0; yItr < imDimY; yItr++)
    {
      std::vector<CalculatorParams *> *lineVector = new std::vector<CalculatorParams *>();
      lineVector->reserve(imDimX);
      for(int xItr = 0; xItr < imDimX; xItr++)
      {
        lineVector->push_back(new CalculatorParams(currentPoint, ++dataPtr, maxIterations));
        currentPoint.r += dx;
      }
      workerThreadJobs.push_back(lineVector);
      currentPoint.i -= dy;
      currentPoint.r = startPoint.r;
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


