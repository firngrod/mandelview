#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include <thread>
#include <mutex>
#include <list> 
#include <fstream>


int precision;

volatile bool mouseEventing;

#include "colors.hpp"
#include "buildimage.hpp"
#include "mandelbrot.hpp"
#include "workerthread.hpp"
#include "saveroutine.hpp"
#include "mouseevents.hpp"
      
int main(int argc, char ** argv)
{
  Json::Value imgDefs;
  Json::Value colDefs;
  for(int i = 1; i < argc; i++)
  {
    std::cout << argv[i] << std::endl;
    std::string thisArg = argv[i];
    int equalsAt = thisArg.find_first_of("=") + 1;
    if(!equalsAt)
      continue;
    std::string argFile = thisArg.substr(equalsAt, thisArg.size() - equalsAt);
    std::ifstream stream(argFile.c_str());
    if(stream.good())
    {
      if(thisArg.find("view=") == 0)
      {
          stream >> imgDefs;
      }
      if(thisArg.find("colors=") == 0)
      {
          stream >> colDefs;
      }
    }
  }

  

  MandelbrotView theView;


  Mandelbrot::CalculateView(theView, imgDefs);
  BuildImage(theView, colDefs);

  //std::string targetFileName = imgDefs.get("TargetFileName", "").asString();
  //if(targetFileName.size() > 0)

  bool didTheThread = false;
  std::string winname = "Mandelview";
  bool quitting = false;
  bool destroyedWindow = true;
  while(!quitting)
  {
    if(destroyedWindow)
    {
      mouseEventing = false;
      theView.redraw = false;
      cv::namedWindow(winname, cv::WINDOW_AUTOSIZE);
      cv::imshow(winname, theView.image);
      cv::setMouseCallback(winname, MouseCallback, &theView);
      destroyedWindow = false;
    }
    int keypress = cv::waitKey(40);
    if(theView.redraw)
    {
      std::cout << "Redrawing\n";
      destroyedWindow = true;
      cv::destroyWindow(winname);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      theView.centerX += (theView.spanX / theView.imDimX) * (theView.downX - theView.imDimX / 2);
      theView.centerY -= (theView.spanY / theView.imDimY) * (theView.downY - theView.imDimY / 2);
      if(theView.fitToX)
      {
        theView.span = theView.span / theView.imDimX * abs(theView.downX - theView.upX) * 2;
      }
      else
      {
        theView.span = theView.span / theView.imDimY * abs(theView.downY - theView.upY) * 2;
      }
      mouseEventing = false;
      theView.upX = -1;
      theView.upY = -1;
      Mandelbrot::CalculateView(theView, theView.Serialize());
      BuildImage(theView, colDefs);
      continue;
    }
    if(mouseEventing)
    {
      SetRectangle(theView);
      cv::imshow(winname, theView.image);
      continue;
    }

    
    if(keypress == 113)
    {
      quitting = true;
    }
    else if(keypress == 115)
    {
      destroyedWindow = true;
      cv::destroyWindow(winname);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      cv::waitKey(1);
      SaveRoutine(theView);
    }

  }

  cv::destroyWindow("Display Window");

  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
