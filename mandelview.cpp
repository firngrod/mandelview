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

  //precision = 40;

  //mpz_class cr = -1, ci = 2;
  //mpz_mul_2exp(cr.get_mpz_t(), cr.get_mpz_t(), precision);
  //mpz_mul_2exp(ci.get_mpz_t(), ci.get_mpz_t(), precision);
  //cr *= ci;
  //mpz_div_2exp(cr.get_mpz_t(), cr.get_mpz_t(), precision);
  //std::cout << cr.get_str(10) << std::endl;
  //std::cout << ci.get_str(10) << std::endl;
  //return 0;

  

  MandelbrotView theView;


  Mandelbrot::ExtractOptions(theView, imgDefs);
  Mandelbrot::CalculateView(theView, true);
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
    int keypress = cv::waitKey(100);
    std::cout << "You pressed " << keypress << std::endl;
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
      Mandelbrot::CalculateView(theView, true);
      BuildImage(theView, colDefs);
      continue;
    }
    if(mouseEventing)
    {
      SetRectangle(theView);
      cv::imshow(winname, theView.image);
      continue;
    }

    
    if(keypress == 113) // q
    {
      quitting = true;
    }
    else if(keypress == 115) // s
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
    //else if(keypress == 122)  // z
    //{
      //destroyedWindow = true;
      //cv::destroyWindow(winname);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //cv::waitKey(1);
      //Mandelbrot::CalculateView(theView, false);
      //BuildImage(theView, colDefs);
    //}
    else if(keypress == 105)  // i
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
      std::cout << "The current iteration limit is: " << theView.maxItr << std::endl;
      std::cout << "Change iteration limit to: ";
      uint64_t newMax = 0;
      while(!newMax)
      {
        std::string newMaxStr;
        std::getline(std::cin, newMaxStr);
        size_t tmp;
        newMax = std::stoull(newMaxStr, &tmp, 10);
      }
      theView.prevMax = theView.maxItr;
      theView.maxItr = newMax;
      Mandelbrot::CalculateView(theView, false);
      BuildImage(theView, colDefs);
    }
    else if(keypress == 114) // r
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
      std::cout << "The current resolution is: " << theView.imDimX << "x" << theView.imDimY << std::endl;
      std::cout << "Enter new resolution:\nNew X: ";
      uint64_t newX = 0, newY = 0;
      while(!newX)
      {
        std::string newMaxStr;
        std::getline(std::cin, newMaxStr);
        size_t tmp;
        newX = std::stoul(newMaxStr, &tmp, 10);
      }
      std::cout << "New Y: ";
      while(!newY)
      {
        std::string newMaxStr;
        std::getline(std::cin, newMaxStr);
        size_t tmp;
        newY = std::stoul(newMaxStr, &tmp, 10);
      }
      theView.imDimX = newX;
      theView.imDimY = newY;
      Mandelbrot::CalculateView(theView, true);
      BuildImage(theView, colDefs);
    }

      
  }

  cv::destroyWindow("Display Window");

  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
