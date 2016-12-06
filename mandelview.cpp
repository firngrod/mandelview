#include <iostream>
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
  imgDefs["Center"]["Real"] =     "-0.7487061230782331046597262297262308";
  imgDefs["Center"]["Imaginary"] = "0.1493926987659020935544189794453501";
  imgDefs["Zoom"] = "4.0e-27";
  imgDefs["Center"]["Real"] =     "-0.5";
  imgDefs["Center"]["Imaginary"] = "0.0";
  imgDefs["Zoom"] = "4.0";
  imgDefs["Fitting"] = "Fit";
  imgDefs["OutputSize"]["x"] = 1920;
  imgDefs["OutputSize"]["y"] = 1200;
  imgDefs["MaxIterations"] = 3000;
  imgDefs["NumThreads"] = 8;
  imgDefs["Passes"] = 4;
  imgDefs["TargetFileName"] = "cheattest1.png";
  
  Json::Value colDefs;
  colDefs["Colors"][0] = 0xFF0000;
  colDefs["Colors"][1] = 0;
  colDefs["Colors"][2] = 0x00FF00;
  colDefs["Colors"][3] = 0;
  colDefs["Colors"][4] = 0x0000FF;
  colDefs["Colors"][5] = 0;
  colDefs["IterationSpan"] = 100;

  MandelbrotView theView;

  Mandelbrot::CalculateView(theView, imgDefs);

  cv::Mat image;

  BuildImage(image, theView, colDefs);

  std::string targetFileName = imgDefs.get("TargetFileName", "").asString();
  if(targetFileName.size() > 0)
    cv::imwrite(targetFileName, image);
  cv::namedWindow("Display Window", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display Window", image);


  cv::waitKey(0);

  //std::cout << "Iterations to escape: " << iterationsToEscape << std::endl;
  //std::cout << (escaped ? "Escaped" : "Not escaped") << std::endl;
  return 0;
}
