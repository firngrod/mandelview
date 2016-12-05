#pragma once
namespace FirnLibs
{
  template <typename T>
  class CyclicVar
  {
  public:
    CyclicVar()
    {
    }


    CyclicVar(const T &max)
    {
      this->max = max;
    }


    CyclicVar(const T &max, const T &start)
    {
      this->max = max;
      this->val = start;
    }


    CyclicVar(const T &max, const T &start, const T &min)
    {
      this->min = min;
      this->max = max;
      this->val = start;
    }


    CyclicVar &operator++()
    {
      if(val == max)
        val = min;
      else
        val++;

      //std::cout << val << std::endl;

      return *this;
    }


    CyclicVar &operator--()
    {
      if(val == min)
      val = max;
      else
      val--;

      return *this;
    }


    operator T() const 
    { 
      return val; 
    };


    T &LookAhead(T &out, T interval) const
    {
      interval %= max - min;
      out = val + interval;
      if(out > max)
      out -= max - min;
      if(out < min)
      out += max - min;

      return out;
    }
  private:
    T min, max, val;
  };
}
