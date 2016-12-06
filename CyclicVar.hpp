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
      this->steps = 1;
    }


    CyclicVar(const T &max, const T &start)
    {
      this->max = max;
      this->val = start;
      this->steps = 1;
    }


    CyclicVar(const T &max, const T &start, const T &min)
    {
      this->min = min;
      this->max = max;
      this->val = start;
      this->steps = 1;
    }


    CyclicVar(const T &max, const T &start, const T &min, const int &steps)
    {
      this->min = min;
      this->max = max;
      this->val = start;
      this->steps = steps;
    }


    CyclicVar &operator++()
    {
      for(int i = 0; i < steps; i++)
      {
        if(val == max)
          val = min;
        else
          val++;
      }
      return *this;
    }


    CyclicVar &operator--()
    {
      for(int i = 0; i < steps; i++)
      {
        if(val == min)
        val = max;
        else
        val--;
      }
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
    int steps;
  };
}
