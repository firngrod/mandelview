#include "buildimage.hpp"
#include "colors.hpp"

void BuildImage(cv::Mat &image, const MandelbrotView &theView, const Json::Value &colDefs)
{
  std::vector<Color> colors;
  CalculateColors(colors, colDefs);
  
  image = cv::Mat(theView.imDimY, theView.imDimX, CV_8UC3, cv::Scalar(0,0,0));

  const uint64_t * dataPtr = &theView.data[0];
  --dataPtr;
  Color * imgPtr = (Color *)image.data;
  --imgPtr;
  for(int i = 0; i < theView.imDimX * theView.imDimY; i++)
  {
    if(*(++dataPtr) != 0)
    {
      *(++imgPtr) = colors[*(dataPtr) % colors.size()];
    }
    else
    {
      *(++imgPtr) = Color(0);
    }
  }
}
