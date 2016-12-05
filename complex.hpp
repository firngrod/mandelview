#pragma once
#include <mpreal.h>
#include <gmpxx.h>
class Complex
{
public:
  Complex();
  Complex(const mpf_class &r, const mpf_class &i);
  mpf_class r, i;

  static int Square(Complex &pout, const Complex &p1, mpf_class &tmpBuf);
  Complex &operator+=(const Complex &other);
  void printToScreen();
};
