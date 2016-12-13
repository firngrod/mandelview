#pragma once
#include <vector>
#include <json/json.h>
#include <opencv2/core/mat.hpp>
#include <sstream>
#include "complex.hpp"


struct MandelbrotView
{
  int imDimX, imDimY, paddedDimX, paddedDimY;
  uint64_t maxItr;
  int numThreads;
  mpf_class span, centerX, centerY;
  std::string fitting;
  int passes;
  mpf_class spanX, spanY;

  std::vector<uint64_t> data;
  cv::Mat image;

  int downX, downY, upX, upY;
  int prevX, prevY;
  volatile bool redraw;
  bool fitToX;
  volatile uint64_t prevMax;

  MandelbrotView(): span(precision), centerX(precision), centerY(precision), spanX(precision), spanY(precision)
  {
    prevMax = 0;
  }

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
    bool negative = number < 0;
    mpf_class positiveFoSho(number);
    positiveFoSho *= (negative ? -1 : 1);
    ss << (negative ? "-" : "") << "0." << positiveFoSho.get_str(exp, 10, 0) << "e";
    ss << exp;
    return ss.str();
  }
};

namespace Mandelbrot
{
  bool CountIterations(uint64_t &iterations, const Complex &point, const uint64_t &maxIterations, mpf_class &tmpBuf);
    //mpf_class &r, mpf_class &i, mpf_class &rSqr, mpf_class &iSqr, mpf_class &summie);
  void CalculateView(MandelbrotView &viewOut, const bool &redraw);
  void ExtractOptions(MandelbrotView &viewOut, const Json::Value &viewDefs);
}

