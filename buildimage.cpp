#include "buildimage.hpp"
#include "colors.hpp"

void BuildImage(cv::Mat &image, const MandelbrotView &theView, const Json::Value &colDefs)
{
  std::vector<Color> colors;
  CalculateColors(colors, colDefs);
  Color black(0);
  
  image = cv::Mat(theView.imDimY, theView.imDimX, CV_8UC3, cv::Scalar(0,0,0));
  int padSize = theView.paddedDimX - theView.imDimX;

  std::vector<uint64_t>::const_iterator dataPtr = theView.data.begin();
  --dataPtr;
  Color * imgPtr = (Color *)image.data;
  --imgPtr;
  for(int i = 0; i < theView.imDimY; i++)
  {
    for(int j = 0; j < theView.imDimX; j++)
    {
      if(*(++dataPtr) < theView.maxItr && *dataPtr > 3)
      {
        *(++imgPtr) = colors[*(dataPtr) % colors.size()];
      }
      else
      {
        *(++imgPtr) = black;
      }
    }
    dataPtr += padSize; 
  }
}
