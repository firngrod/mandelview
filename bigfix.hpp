#pragma once
#include <vector>
#include <sstream>
#include <gmpxx.h>
#include <iostream>
extern int fixPrecision;

class BigFix
{
public:

  inline static void Add(BigFix &out, const BigFix &input1, const BigFix &input2) 
  { 
    if(input1.isNegative == input2.isNegative)
    {
      out.__Add(input1, input2);
      out.isNegative = input1.isNegative;
    }
    else
    {
      if(input1.IsGreaterThan(input2) || input1.IsEqualTo(input2))
      {
        out.__Subtract(input1, input2);
        out.isNegative = input1.isNegative;
      }
      else
      {
        out.__Subtract(input2, input1);
        out.isNegative = !input1.isNegative;
      }
    }
  }

  inline void static Subtract(BigFix &out, const BigFix &input1, const BigFix &input2)
  {
    if(input1.isNegative != input2.isNegative)
    {
      out.__Add(input1, input2);
      out.isNegative = input1.isNegative;
      return;
    }
    else
    {
      if(input1.IsGreaterThan(input2) || input1.IsEqualTo(input2))
      {
        out.__Subtract(input1, input2);
        out.isNegative = input1.isNegative;
      }
      else
      {
        out.__Subtract(input2, input1);
        out.isNegative = !input1.isNegative;
      }
    }
  }

  inline void __Add(const BigFix &input1, const BigFix &input2)
  {
    tmpForCarry = 0;
    for(int i = 0; i < vecSize; ++i)
    {
      dataVec[i] = tmpForCarry;
      tmpForCarry = CarryAdd(dataVec[i], input1.dataVec[i]);
      tmpForCarry += CarryAdd(dataVec[i], input2.dataVec[i]);
    }
  }
      

  static inline int CarryAdd(uint64_t &out, const uint64_t &in)
  {
    out += in;
    return out < in;
  }

  inline void __Subtract(const BigFix &input1, const BigFix &input2)
  {
    tmpForCarry = 0;
    for(int i = 0; i < vecSize; i++)
    {
      dataVec[i] = input1.dataVec[i];
      tmpForCarry = CarrySub(dataVec[i], tmpForCarry);
      tmpForCarry += CarrySub(dataVec[i], input2.dataVec[i]);
    }
  }

  static inline int CarrySub(uint64_t &out, const uint64_t &in)
  {
    int carry = out < in;
    out -= in;
    return carry;
  }

  inline int IsGreaterThan(const BigFix &other) const
  {
    int i = 0;
    for(; i < vecSize; i++)
    {
      if(dataVec[vecSize1 - i] != other.dataVec[other.vecSize1 - i])
        break;
    }
    return dataVec[vecSize1 - i] > other.dataVec[other.vecSize1 - i];
  }

  inline bool IsEqualTo(const BigFix &other) const
  {
    for(int i = 0; i < vecSize; i++)
    {
      if(dataVec[vecSize1 - i] != other.dataVec[other.vecSize1 - i])
        return false;
    }
    return true;
  }

  inline static void Square(BigFix &out, const BigFix &input)
  {
    out.isNegative = false;

    out.tmpForCarry = 0;
    bool even = true;
    out.Zero();
    int j;
    for(int i = 0 ; i < out.vecSize + 1; ++i)
    {
      out.toAdd = even ? (unsigned __int128)input.dataVec[input.vecSize1 - i/2] * input.dataVec[input.vecSize1 - i/2] : 0;
      even = !even;
      out.tmpForCarry = 0;
      for(j = 0; j < (i + 1) / 2; ++j)
      {
        out.tmpForCarry += CarryAdd(out.toAdd, (unsigned __int128)input.dataVec[input.vecSize1 - j] * input.dataVec[input.vecSize1 - i + j]);
        out.tmpForCarry += CarryAdd(out.toAdd, (unsigned __int128)input.dataVec[input.vecSize1 - j] * input.dataVec[input.vecSize1 - i + j]);
      }
      out.__Add(out.toAdd, out.tmpForCarry, out.vecSize1 - i);
    }
  }

  inline void __Add(const unsigned __int128 &in, const int &carry, int pos)
  {
    int localCarry = 0;
    
    if(pos >= 0)
    {
      localCarry = CarryAdd(dataVec[pos], in & 0xFFFFFFFFFFFFFFFF);
    }
    if(pos < vecSize1)
    {
      localCarry = CarryAdd(dataVec[++pos], localCarry);
      localCarry += CarryAdd(dataVec[pos], in >> 64);
    }
    localCarry += carry;
    while(localCarry && ++pos < vecSize)
    {
      localCarry = CarryAdd(dataVec[pos], localCarry);
    }
  }

  static inline int CarryAdd(unsigned __int128 &out, const unsigned __int128 &in)
  {
    out += in;
    return out < in;
  }

  inline const int &VecSize() const { return vecSize; }

  inline void Zero()
  {
    for(std::vector<uint64_t>::iterator vecItr = dataVec.begin(), vecEnd = dataVec.end(); vecItr != vecEnd; vecItr++)
      *vecItr = 0;
    isNegative = false;
  }

  BigFix()
  {
    dataVec.resize(fixPrecision, 0);
    vecSize = fixPrecision;
    vecSize1 = fixPrecision - 1;
  }

  BigFix(const std::string &numString)
  {
    dataVec.resize(fixPrecision, 0);
    vecSize = fixPrecision;
    vecSize1 = fixPrecision - 1;
    mpf_class cheating(numString, 1000);
    SetMpfClass(cheating);
  }

  BigFix(mpf_class &cheating)
  {
    dataVec.resize(fixPrecision, 0);
    vecSize = fixPrecision;
    vecSize1 = fixPrecision - 1;
    SetMpfClass(cheating);
  }

  inline void SetMpfClass(const mpf_class &cheating)
  {
    Zero();
    isNegative = cheating < 0;
    mp_exp_t exp;
    std::string unreasonablySlowMethod = cheating.get_str(exp, 2, 10000);
    std::string::iterator strItr = unreasonablySlowMethod.begin(), strEnd = unreasonablySlowMethod.end();
    //std::cout << unreasonablySlowMethod << " " << cheating.get_prec() << std::endl;
    if(isNegative)
      ++strItr;
    for(int i = exp - 1; (i >= 0) && (strItr != strEnd); --i)
    {
      dataVec[vecSize1] += (*strItr) == '1' ? (1 << i) : 0;
      ++strItr;
    }
    for(int i = 63 + std::min((mp_exp_t)0, exp) % 64, j = abs(std::min((mp_exp_t)0, exp)) / 64; strItr != strEnd; --i)
    {
      dataVec[vecSize - 2 - j] += (*strItr) == '1' ? ((uint64_t)1 << i) : 0;
      ++strItr;
      if(!i)
      {
        i = 64;
        ++j;
      }
      if(j > vecSize - 2)
        break;
    }
  }

  //BigFix(const BigFix &other)
  //{
    //*this = other;
  //}

  //inline BigFix &operator=(const BigFix &other)
  //{
    //dataVec = other.dataVec;
    //isNegative = other.isNegative;
    //vecSize = other.vecSize;
    //vecSize1 = vecSize - 1;
  //}  

  std::string ToString() const
  {
    mpf_class tmp(0, vecSize * 64);
    for(int i = 0; i < vecSize1; ++i)
    {
      tmp += dataVec[i];
      tmp /= (uint64_t)1 << 32;
      tmp /= (uint64_t)1 << 32;
    }
    tmp += dataVec[vecSize1];
     
    if(isNegative)
      tmp *= -1;

    return Stringifympf(tmp);
  }


  std::string Stringifympf(const mpf_class &number) const
  {
    mp_exp_t exp;
    std::stringstream ss;
    bool negative = number < 0;
    mpf_class positiveFoSho(0, number.get_prec());
    positiveFoSho = number * (negative ? -1 : 1);
    ss << (negative ? "-" : "") << "0." << positiveFoSho.get_str(exp) << "e";
    ss << exp;
    return ss.str();
  }

  uint64_t GetInt()
  {
    return dataVec[vecSize1];
  }
  
private:
  std::vector<uint64_t> dataVec;
  int vecSize, vecSize1;
  bool isNegative;
  int tmpForCarry;
  unsigned __int128 toAdd;
};
