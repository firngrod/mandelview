#pragma once
#include <vector>
#include <json/json.h>
#include "complex.hpp"

struct MandelbrotView
{
  int imDimX, imDimY;
  std::vector<uint64_t> data;
};

namespace Mandelbrot
{
  bool CountIterations(uint64_t &iterations, const Complex &point, const uint64_t &maxIterations, mpf_class &tmpBuf);
    //mpf_class &r, mpf_class &i, mpf_class &rSqr, mpf_class &iSqr, mpf_class &summie);
  bool ProvenDivergent(const Complex &p, mpf_class &tmpBuf);
  void CalculateView(MandelbrotView &viewOut, const Json::Value &viewDefs);
}

