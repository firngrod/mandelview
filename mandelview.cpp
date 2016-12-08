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
      if(thisArg.find("startview=") == 0)
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

  cv::Mat image;

  Mandelbrot::CalculateView(theView, imgDefs);
  BuildImage(image, theView, colDefs);

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
      cv::namedWindow(winname, cv::WINDOW_AUTOSIZE);
      cv::imshow(winname, image);
      destroyedWindow = false;
    }
    int keypress = cv::waitKey(40);
    if(mouseEventing)
      continue;
    
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
      SaveRoutine(image, theView);
    }

  }

  cv::destroyWindow("Display Window");

  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
