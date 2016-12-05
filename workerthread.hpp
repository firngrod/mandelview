#pragma once
#include "complex.hpp"
#include <mutex>
#include <list>
#include <vector>

// What a worker thread needs to do one pixel
struct CalculatorParams
{
  Complex point;
  uint64_t * target;
  const uint64_t maxItr;
  CalculatorParams(const Complex &point, uint64_t * target, const uint64_t maxItr): point(point), target(target), maxItr(maxItr){}
};

// What each worker thread needs to run at all
struct WorkerParams
{
  std::mutex *dataMutex;
  std::list<std::vector<CalculatorParams *> *> *workerThreadJobs;
};

void WorkerThread(void *paramsPtr);
