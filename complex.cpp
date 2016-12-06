#include "complex.hpp"
#include <iostream>

extern int precision;

Complex::Complex()
{
  i.set_prec(precision);
  r.set_prec(precision);
}

Complex::Complex(const mpf_class &r, const mpf_class &i): r(r), i(i)
{
}

//int Complex::Square(Complex &pout, const Complex &p1, mpf_class &tmpBuf)
//{

  ////std::cout << "p1 " << p1.r << " + " << p1.i << "i" << std::endl;
  ////std::cout << "p2 " << p2.r << " + " << p2.i << "i" << std::endl;
  //tmpBuf = pout.r = p1.r * p1.r;
  //pout.i = p1.i * p1.i;
  //tmpBuf += pout.i;
  //if(4 < tmpBuf)
    //return 1;
  //pout.r -= pout.i;
  //tmpBuf = p1.r * p1.i;
  //pout.i = tmpBuf + tmpBuf;
  ////std::cout << "pout " << pout.r << " + " << pout.i << "i" << std::endl;
  //return 0;
//}

//Complex &Complex::operator+=(const Complex &other)
//{
  //this->r += other.r;
  //this->i += other.i;
  //return *this;
//}

