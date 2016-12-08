extern int precision;
#include "mandelbrot.hpp"
#include "workerthread.hpp"

uint64_t CalculatorParams::maxItr = 0;
int CalculatorParams::paddedDimX = 0;
uint64_t CalculatorParams::totalPoints = 0;
uint64_t CalculatorParams::pointsDone = 0;
extern int precision;

void WorkerThread(void *paramsPtr)
{
  WorkerParams *params = (WorkerParams *)paramsPtr;
  bool done = false;
  bool down = false;
  bool right = false;
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
      CalculatorParams::pointsDone += calcParamsVec->size();
      std::cout << "Percentage done: " << (CalculatorParams::pointsDone * 100) / CalculatorParams::totalPoints << "%\n";
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
        //(*calcItr)->point;

        //Check if we've calculated this point before.  If we have, it isn't 0
        if(!*(*calcItr)->target)
        {
          // Now check the surrounding points to see if we need to calculate
          int &offset = (*calcItr)->checkOffset;
          if(offset)
          {
            // We know it's safe to look one step to the right, one step down and the combination.

            // First look to the right.
            uint64_t * checkPtr1 = (*calcItr)->target + offset;
            down = false;
            right = true;

            // If not, check down from there
            if(!*checkPtr1)
            {
              checkPtr1 += offset * CalculatorParams::paddedDimX;
              down = true;
              right = true;
            }

            // If not, it has to be left of that, 'cause it's not where we're standing.
            if(!*checkPtr1)
            {
              checkPtr1 -= offset;
              down = true;
              right = false;
            }

            // Now we have a point.  We also know where it was.  This is important.
            uint64_t * checkPtr2 = checkPtr1 + (right ? -1 : 1) * offset * 2;
            uint64_t * checkPtr3 = checkPtr1 + (down ? -1 : 1) * offset * 2 * CalculatorParams::paddedDimX;
            uint64_t * checkPtr4 = checkPtr3 + (right ? -1 : 1) * offset * 2;

            // Finally, now that we have the four points, we need to compare them.  Real quick and nasty:  Xor them bitwise and see if we get 0.
            // If we do, assign the same value to this point.
            if( *checkPtr1 == *checkPtr2 &&
                *checkPtr1 == *checkPtr3 &&
                *checkPtr1 == *checkPtr4)
              *(*calcItr)->target = *checkPtr1;
          }
          if(!*(*calcItr)->target)
            escaped = Mandelbrot::CountIterations(*(*calcItr)->target, (*calcItr)->point, CalculatorParams::maxItr, tmpBuf);
        }
        //std::cout << "Did iterations: " << *(*calcItr)->target << std::endl;
        delete (*calcItr);
      }
      delete calcParamsVec;
    }
  }
  std::cout << "Worker done, terminating.\n";
  delete params;
}
