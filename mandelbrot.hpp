#pragma once
#include <vector>
#include <json/json.h>
#include <opencv2/core/mat.hpp>
#include <sstream>
#include "complex.hpp"
#include "cryptozip.hpp"


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

  Json::Value Serialize(const bool &calculatedData = false) const
  {
    Json::Value out;
    out["Center"]["Real"] = Stringifympf(centerX);
    out["Center"]["Imaginary"] = Stringifympf(centerY);
    out["Span"] = Stringifympf(span);
    out["Fitting"] = fitting;
    out["OutputSize"]["X"] = imDimX;
    out["OutputSize"]["Y"] = imDimY;
    out["MaxIterations"] = maxItr;
    out["Passes"] = passes;
    if(calculatedData)
    {
      std::string zippedData;
      FirnLibs::Crypto::ZipEncode(zippedData, (unsigned char *)&data[0], data.size() * 8);
      out["Calculated Data"] = zippedData;
      out["Calculated Data Size"] = data.size();
    }
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
  void CountIterations(uint64_t &iterations, const mpz_class &cr, const mpz_class &ci, const uint64_t &maxIterations,
    mpz_class &zr, mpz_class &zi, mpz_class &zrsqr, mpz_class &zisqr, const mpz_class &bailout, mpz_class &tmp);
  void CalculateView(MandelbrotView &viewOut, const bool &redraw);
  void ExtractOptions(MandelbrotView &viewOut, const Json::Value &viewDefs);
}

