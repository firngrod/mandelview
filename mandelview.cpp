#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include <thread>
#include <mutex>
#include <list> 
#include "mandelbrot.hpp"
#include "workerthread.hpp"
#include "colors.hpp"
#include "buildimage.hpp"


int precision;

      
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
  //imgDefs["Center"]["Real"] =     "-0.7487061230782331046597262297262308";
  //imgDefs["Center"]["Imaginary"] = "0.1493926987659020935544189794453501";
  //imgDefs["Span"] = "4.0e-27";
  ////imgDefs["Center"]["Real"] =     "-0.5";
  ////imgDefs["Center"]["Imaginary"] = "0.0";
  ////imgDefs["Zoom"] = "3.0";
  //imgDefs["Fitting"] = "Fit";
  //imgDefs["OutputSize"]["X"] = 1920;
  //imgDefs["OutputSize"]["Y"] = 1200;
  //imgDefs["MaxIterations"] = 3000;
  //imgDefs["NumThreads"] = 8;
  //imgDefs["Passes"] = 4;
  //imgDefs["TargetFileName"] = "cheattest";
  

  MandelbrotView theView;

  cv::Mat image;

  Mandelbrot::CalculateView(theView, imgDefs);
  BuildImage(image, theView, colDefs);

  //std::string targetFileName = imgDefs.get("TargetFileName", "").asString();
  //if(targetFileName.size() > 0)
    //cv::imwrite(targetFileName, image);
  cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display Window", image);

  bool quitting = false;
  while(!quitting)
  {
    int keypress = cv::waitKey(0);
    std::cout << "You pressed: " << keypress << std::endl;
    if(keypress == 113)
      quitting = true;
  }

  cv::destroyWindow("Display Window");

  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
