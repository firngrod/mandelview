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
extern int fixPrecision;

volatile bool mouseEventing;

#include "bigfix.hpp"
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

  fixPrecision = 3;

  //BigFix testie("4.0e-27"), testie2("1.0"), testie3;
  //BigFix::Add(testie3, testie, testie2);
  //std::cout << testie.ToString() << std::endl;
  //unsigned char moo = 230;
  //while(++moo)
  //{
    //BigFix::Square(testie2, testie3);
    //BigFix::Square(testie3, testie2);
    //std::cout << testie2.ToString() << std::endl;
  //}

  //std::cout << testie3.ToString() << std::endl;
  //return 0;
  //return 0;
  
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

  std::cout << fixPrecision * 64 << std::endl;
  std::cout << precision << std::endl;
  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
