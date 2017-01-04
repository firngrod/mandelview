#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include <thread>
#include <mutex>
#include <list> 
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>


int precision;

volatile bool mouseEventing;

#include "colors.hpp"
#include "buildimage.hpp"
#include "mandelbrot.hpp"
#include "workerthread.hpp"
#include "saveroutine.hpp"
#include "mouseevents.hpp"
#include "cryptozip.hpp"
#include "halfspan.hpp"
      
int main(int argc, char ** argv)
{
  std::string winname = "Mandelview";
  Json::Value imgDefs;
  Json::Value colDefs;
  bool doZoom = false;
  std::string zoomName;
  for(int i = 1; i < argc; i++)
  {
    if(std::string(argv[i]) == "zoomto")
      doZoom = true;
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
          zoomName = argFile + ".zoomOutput";
      }
      if(thisArg.find("colors=") == 0)
      {
          stream >> colDefs;
      }
    }
  }

  MandelbrotView theView;
  Mandelbrot::ExtractOptions(theView, imgDefs);

  if(doZoom)
  {
    mpf_class finalZoom(theView.span);
    theView.span = 4;
    cv::namedWindow(winname, cv::WINDOW_AUTOSIZE);
    std::cout << "Trying to make dir:  " << zoomName << std::endl;
    mkdir(zoomName.c_str(), 0700);
    uint64_t highItr;
    int fileNumber = 0;
    while(theView.span > finalZoom)
    {
      char fileName[0x1000], jsonName[0x1000];
      sprintf(jsonName, "%s/%09d.json", zoomName.c_str(), fileNumber);
      std::ifstream testieStream(jsonName);
      if(testieStream.good())
      {
        std::cout << "Opening existing json.\n";
        Json::Value tmpJson;
        testieStream >> tmpJson;
        MandelbrotView tmpView;
        Mandelbrot::ExtractOptions(tmpView, tmpJson);
        theView.data = tmpView.data;
        theView.maxItr = tmpView.maxItr;
        theView.prevMax = tmpView.prevMax;
        Mandelbrot::CalculateView(theView, false, true);
      }
      else
      {
        Mandelbrot::CalculateView(theView, true);
      }
      BuildImage(theView, colDefs, &highItr);
      cv::imshow(winname, theView.image);
      sprintf(fileName, "%s/%09d.png", zoomName.c_str(), fileNumber);
      cv::imwrite(fileName, theView.image);
      std::ofstream outStream(jsonName);
      outStream << theView.Serialize(true).toStyledString();
      int keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      keypress = cv::waitKey(1);
      theView.span *= 0.95;
      theView.maxItr = highItr * 5;
      fileNumber++;
    }
    return 0;
  }
     

  Mandelbrot::CalculateView(theView, !theView.data.size());
  BuildImage(theView, colDefs);

  //std::string targetFileName = imgDefs.get("TargetFileName", "").asString();
  //if(targetFileName.size() > 0)

  bool didTheThread = false;
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
    else if(keypress == 122)  // z
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
      HalfSpan(theView);

      Mandelbrot::CalculateView(theView, false);
      BuildImage(theView, colDefs);
    }
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
        try
        {
          newMax = std::stoull(newMaxStr, &tmp, 10);
        }
        catch (...)
        {}
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
        try
        {
          newX = std::stoul(newMaxStr, &tmp, 10);
        }
        catch (...)
        {}
      }
      std::cout << "New Y: ";
      while(!newY)
      {
        std::string newMaxStr;
        std::getline(std::cin, newMaxStr);
        size_t tmp;
        try
        {
          newY = std::stoul(newMaxStr, &tmp, 10);
        }
        catch (...)
        {}
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
