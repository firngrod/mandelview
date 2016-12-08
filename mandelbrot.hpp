#pragma once
#include <vector>
#include <json/json.h>
#include "complex.hpp"


struct MandelbrotView
{
  int imDimX, imDimY, paddedDimX, paddedDimY;
  std::vector<uint64_t> data;
  uint64_t maxItr;
  int numThreads;
  mpf_class span, centerX, centerY;
  std::string fitting;
  int passes;

  MandelbrotView(): span(precision), centerX(precision), centerY(precision)
  {}

  Json::Value Serialize() const
  {
    Json::Value out;
    out["Center"]["Real"] = Stringifympf(centerX);
    out["Center"]["Imaginary"] = Stringifympf(centerY);
    out["Span"] = Stringifympf(span);
    out["Fitting"] = fitting;
    out["OutputSize"]["X"] = imDimX;
    out["OutputSize"]["Y"] = imDimY;
    out["MaxIterations"] = maxItr;
    out["NumThreads"] = numThreads;
    out["Passes"] = passes;
    return out;
  }

  std::string Stringifympf(const mpf_class &number) const
  {
    mp_exp_t exp;
    std::stringstream ss;
    ss << "0." << number.get_str(exp) << "e";
    ss << exp - 1;
    return ss.str();
  }
};

namespace Mandelbrot
{
  bool CountIterations(uint64_t &iterations, const Complex &point, const uint64_t &maxIterations, mpf_class &tmpBuf);
    //mpf_class &r, mpf_class &i, mpf_class &rSqr, mpf_class &iSqr, mpf_class &summie);
  bool ProvenDivergent(const Complex &p, mpf_class &tmpBuf);
  void CalculateView(MandelbrotView &viewOut, const Json::Value &viewDefs);
}

