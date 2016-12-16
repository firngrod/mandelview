#include <iostream>
#include <string>

extern int precision;
extern volatile bool mouseEventing;
#include "mandelbrot.hpp"

void MouseCallback(int event, int x, int y, int flags, void *userdata)
{
  MandelbrotView &theView = *(MandelbrotView *)userdata;
  if(event == 1)
  {
    mouseEventing = true;
    theView.downX = x - 2;
    theView.downY = y - 4;
  }
  if(event == 4)
  {
    theView.redraw = true;
  }
  theView.upX = x - 2;
  theView.upY = y - 4;
}
