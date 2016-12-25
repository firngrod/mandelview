extern int precision;
#include "mandelbrot.hpp"
#include "halfspan.hpp"
#include <iostream>

void HalfSpan(MandelbrotView &theView)
{
  std::vector<uint64_t> &data = theView.data;
  theView.span /= 2;
  theView.spanX /= 2;
  theView.spanY /= 2;

  for(int upperOuter = 0; upperOuter < theView.imDimY / 2 + 1; ++upperOuter)
  {
    if(upperOuter% 2)
    {
      memset(&data[upperOuter * theView.paddedDimX], 0, theView.paddedDimX * 8);
      memset(&data[(theView.imDimY - upperOuter) * theView.paddedDimX], 0, theView.paddedDimX * 8);
    }
    else
    {
      for(int outerLeft = 0; outerLeft < theView.imDimX / 2 + 1; ++outerLeft)
      {
        if(outerLeft % 2)
        {
          data[upperOuter * theView.paddedDimX + outerLeft] = 0;
          data[upperOuter * theView.paddedDimX + theView.imDimX - outerLeft] = 0;
          data[(theView.imDimY - upperOuter) * theView.paddedDimX + outerLeft] = 0;
          data[(theView.imDimY - upperOuter) * theView.paddedDimX + theView.imDimX - outerLeft] = 0;
        }
        else
        {
          data[upperOuter * theView.paddedDimX + outerLeft] = 
            data[(upperOuter + theView.imDimY / 2) / 2 * theView.paddedDimX + (outerLeft + theView.imDimX / 2) / 2 ];
          data[upperOuter * theView.paddedDimX + theView.imDimX - outerLeft] = 
            data[(upperOuter + theView.imDimY / 2) / 2 * theView.paddedDimX + (theView.imDimX - outerLeft + theView.imDimX / 2) / 2];
          if(upperOuter != theView.imDimY / 2)
          {
            data[(theView.imDimY - upperOuter) * theView.paddedDimX + outerLeft] = 
              data[(theView.imDimY - upperOuter + theView.imDimY / 2) / 2 * theView.paddedDimX + (outerLeft + theView.imDimX / 2) / 2];
            data[(theView.imDimY - upperOuter) * theView.paddedDimX + theView.imDimX - outerLeft] = 
              data[(theView.imDimY - upperOuter + theView.imDimY / 2) / 2 * theView.paddedDimX + (theView.imDimX - outerLeft + theView.imDimX / 2) / 2];
          }
        }
      }
    }
  }
  for(int i = 0; i < theView.paddedDimY; i++)
  {
    for(int j = (i >= theView.imDimY) ? 0 : theView.imDimX; j < theView.paddedDimX; j++)
    {
      data[i * theView.paddedDimX + j] = 0;
    }
  }
}
