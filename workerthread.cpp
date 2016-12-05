#include "mandelbrot.hpp"
#include "workerthread.hpp"

extern int precision;

void WorkerThread(void *paramsPtr)
{
  WorkerParams *params = (WorkerParams *)paramsPtr;
  bool done = false;
  std::vector<CalculatorParams *> *calcParamsVec;
  bool escaped;
  mpf_class tmpBuf(0, precision);
  //mpf_class iSqr(0, precision), rSqr(0, precision), i(0, precision), r(0, precision), summie(0, precision);
  std::unique_lock<std::mutex> queueLock(*params->dataMutex);
  queueLock.unlock();
  while(!done)
  {
    queueLock.lock();
    if(params->workerThreadJobs->size() > 0)
    {
      std::list<std::vector<CalculatorParams *> *>::iterator prmItr = params->workerThreadJobs->begin();
      calcParamsVec = *prmItr;
      std::cout << "Lines left: " << params->workerThreadJobs->size() << std::endl;
      params->workerThreadJobs->erase(prmItr);
    }
    else
    {
      queueLock.unlock();
      return;
    }
    queueLock.unlock();
    if(calcParamsVec)
    {
      for(std::vector<CalculatorParams *>::iterator calcItr = calcParamsVec->begin(), calcEnd = calcParamsVec->end(); calcItr != calcEnd; calcItr++)
      {
        (*calcItr)->point;
        //escaped = Mandelbrot::CountIterations(*(*calcItr)->target, (*calcItr)->point, (*calcItr)->maxItr, r, i, rSqr, iSqr, summie);
        escaped = Mandelbrot::CountIterations(*(*calcItr)->target, (*calcItr)->point, (*calcItr)->maxItr, tmpBuf);
        //std::cout << "Did iterations: " << *(*calcItr)->target << std::endl;
        if(!escaped)
          *((*calcItr)->target) = 0;
        delete (*calcItr);
      }
      delete calcParamsVec;
    }
  }
  std::cout << "Worker done, terminating.\n";
  delete params;
}
