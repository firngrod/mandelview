#include "colors.hpp"
void CalculateColors(std::vector<Color> &colors, const Json::Value &colDefs)
{
  Json::Value jvColors = colDefs.get("Colors", Json::Value());
  jvColors[jvColors.size()] = jvColors[0];
  double iterationSpan = colDefs.get("IterationSpan", 20).asInt();
  colors.resize((size_t)iterationSpan);
  int outColNo = 0;
  double spanPerColor = iterationSpan / (jvColors.size() - 1);

  for(double inColNo = 0; inColNo < (double)jvColors.size() - 1; inColNo++)
  {
    int thisCol = jvColors[(int)inColNo].asInt();
    int nextCol = jvColors[(int)inColNo + 1].asInt();
    double offset = inColNo * spanPerColor;
    while(outColNo < offset + spanPerColor)
    {
      double weight = ((double)outColNo - offset) / spanPerColor;
      colors[outColNo] = Color(thisCol, nextCol, weight);
      ++outColNo;
    }
  }
}
