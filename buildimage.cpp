extern int precision;
#include "buildimage.hpp"
#include "colors.hpp"

void BuildImage(MandelbrotView &theView, Json::Value colDefs)
{
  int iterationSpan = colDefs.get("IterationSpan", 0).asUInt();
  if(!iterationSpan)
  {
    std::vector<uint64_t> forSorting;
    forSorting.reserve(theView.data.size());
    for(std::vector<uint64_t>::const_iterator datItr = theView.data.begin(), datEnd = theView.data.end(); datItr != datEnd; datItr++)
    {
      if(*datItr > 0 && *datItr < theView.maxItr)
      {
        forSorting.push_back(*datItr);
      }
    }
    std::sort(forSorting.begin(), forSorting.end());
    uint64_t closeToMin = forSorting[forSorting.size() / 100];
    uint64_t closeToMax = forSorting[forSorting.size() - forSorting.size() / 100];
    colDefs["IterationSpan"] = closeToMax - closeToMin;
    colDefs["Offset"] = closeToMin;
  }

  std::vector<Color> colors;
  CalculateColors(colors, colDefs);
  Color black(0);
  
  theView.image = cv::Mat(theView.imDimY, theView.imDimX, CV_8UC3, cv::Scalar(0,0,0));
  int padSize = theView.paddedDimX - theView.imDimX;

  std::vector<uint64_t>::const_iterator dataPtr = theView.data.begin();
  --dataPtr;
  Color * imgPtr = (Color *)theView.image.data;
  --imgPtr;
  for(int i = 0; i < theView.imDimY; i++)
  {
    for(int j = 0; j < theView.imDimX; j++)
    {
      if(*(++dataPtr) < theView.maxItr && *dataPtr > 2)
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


void InvertRectangleAroundPoint(int centerX, int centerY, int mouseX, int mouseY, MandelbrotView &theView)
{
  int dx = theView.imDimX / std::max(abs(centerX - mouseX), 1);
  int dy = theView.imDimY / std::max(abs(centerY - mouseY), 1);
  theView.fitToX = dy > dx;
  if(theView.fitToX)
  {
    dx = abs(centerX - mouseX);
    dy = dx * theView.imDimY / theView.imDimX;
  }
  else
  {
    dy = abs(centerY - mouseY);
    dx = dy * theView.imDimX / theView.imDimY;
  }

  int top = centerY - dy;
  int bottom = centerY + dy;
  int left = centerX - dx;
  int right = centerX + dx;

  uint64_t offset = top * theView.imDimX;
  // Draw the top
  if(top >= 0 && top < theView.imDimY)
    for(int x = left; x <= right; x++)
    {
      if(x >= 0 && x < theView.imDimX)
        ((Color *)theView.image.data + offset + x)->Invert();
    }
  // Draw the bottom
  offset = bottom * theView.imDimX;
  if(bottom > 0 && bottom < theView.imDimY)
    for(int x = left; x <= right; x++)
    {
      if(x >= 0 && x < theView.imDimX)
        ((Color *)theView.image.data + offset + x)->Invert();
    }
  // Draw the left
  if(left >= 0 && left < theView.imDimX)
    for(int y = top + 1; y < bottom; y++)
    {
      if(y >= 0 && y < theView.imDimY)
        ((Color *)theView.image.data + y * theView.imDimX + left)->Invert();
    }
  // Draw the right
  if(right > 0 && right < theView.imDimX)
    for(int y = top + 1; y < bottom; y++)
    {
      if(y >= 0 && y < theView.imDimY)
        ((Color *)theView.image.data + y * theView.imDimX + right)->Invert();
    }
}

void SetRectangle(MandelbrotView &theView)
{
  if(theView.downX == theView.upX && theView.downY == theView.upY)
    return;

  if(theView.prevX > 0)
    InvertRectangleAroundPoint(theView.downX, theView.downY, theView.prevX, theView.prevY, theView);

  theView.prevX = theView.upX;
  theView.prevY = theView.upY;

  if(theView.prevX > 0)
    InvertRectangleAroundPoint(theView.downX, theView.downY, theView.prevX, theView.prevY, theView);

}
