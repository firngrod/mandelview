#include <string>
#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/highgui/highgui.hpp>
extern int precision;
#include "mandelbrot.hpp"

void SaveRoutine(const MandelbrotView &theView)
{
  std::string saveType;
  while(saveType != "1" && saveType != "2" && saveType != "c")
  {
    std::cout << "Save what?\n 1) Image file\n 2) Image parameters (For later re-drawing)\n\nChoice (number or c): ";
    std::getline(std::cin, saveType);
  }

  if(saveType == "c")
    return;

  bool filenameGood = false, cancelled = false;
  std::string filename;
  while(!filenameGood && !cancelled)
  {
    std::cout << "Enter filename to save:  ";
    std::getline(std::cin, filename);
    if(filename == "")
    {
      cancelled = true;
    }
    else
    {
      std::ifstream testieStream(filename);
      if(testieStream.good())
      {
        std::cout << "File exists, overwrite? (y/n/c)";
        std::string overwriteAnswer;
        while(overwriteAnswer == "")
        {
          overwriteAnswer = std::getchar();
          if(overwriteAnswer == "y")
          {
            filenameGood = true;
          }
          if(overwriteAnswer == "c")
          {
            cancelled = true;
          }
          if(overwriteAnswer == "n")
          {
            overwriteAnswer = "";
          }
        }
      }
      else
      {
        std::ofstream testieStream(filename);
        if(!testieStream.good())
        {
          std::cout << "Could not open file for writing.\n";
        }
        else
        {
          filenameGood = true;
        }
      }
    }
  }
  if(filenameGood)
  {
    if(saveType == "1")
    {
      cv::imwrite(filename, theView.image);
    }
    else if(saveType == "2")
    {
      std::ofstream outStream(filename);
      outStream << theView.Serialize().toStyledString();
    }
  }
}
