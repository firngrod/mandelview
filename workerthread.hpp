#pragma once
#include "complex.hpp"
#include <mutex>
#include <list>
#include <vector>

// What a worker thread needs to do one pixel
struct CalculatorParams
{
public:
  Complex point;
  uint64_t * target;
  int checkOffset;

  const uint64_t &getMaxItr() const { return maxItr;}
  void setMaxItr(const uint64_t &maxItrIn) { maxItr = maxItrIn; }
  CalculatorParams(const Complex &point, uint64_t * target, int checkOffset): point(point), target(target), checkOffset(checkOffset){}
  static uint64_t maxItr;
  static int paddedDimX;
  static uint64_t totalPoints;
  static uint64_t pointsDone;
};

// What each worker thread needs to run at all
struct WorkerParams
{
  std::mutex *dataMutex;
  std::list<std::vector<CalculatorParams *> *> *workerThreadJobs;
};

void WorkerThread(void *paramsPtr);
