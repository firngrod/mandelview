#include "colors.hpp"
void CalculateColors(std::vector<Color> &colors, Json::Value colDefs)
{
  if(colDefs.type() == Json::ValueType::nullValue)
  {
    colDefs["Colors"][0] = "00ff00";
    colDefs["Colors"][1] = "0";
    colDefs["Colors"][2] = "0000ff";
    colDefs["Colors"][3] = "0";
    colDefs["Colors"][4] = "ff0000";
    colDefs["Colors"][5] = "0";
    colDefs["IterationSpan"] = 100;
    colDefs["Offset"] = 0;
  }

  Json::Value jvColors = colDefs.get("Colors", Json::Value());
  jvColors[jvColors.size()] = jvColors[0];
  double iterationSpan = colDefs.get("IterationSpan", 100).asInt();
  int cycleOffset = colDefs.get("Offset", 0).asInt();
  colors.resize((size_t)iterationSpan);
  int outColNo = 0;
  double spanPerColor = iterationSpan / (jvColors.size() - 1);

  for(double inColNo = 0; inColNo < (double)jvColors.size() - 1; inColNo++)
  {
    int thisCol = std::stoi(jvColors[(int)inColNo].asString(), NULL, 16);
    int nextCol = std::stoi(jvColors[(int)inColNo + 1].asString(), NULL, 16);
    double offset = inColNo * spanPerColor;
    while(outColNo < offset + spanPerColor)
    {
      double weight = ((double)outColNo - offset) / spanPerColor;
      colors[(outColNo + cycleOffset) % colors.size()] = Color(thisCol, nextCol, weight);
      ++outColNo;
    }
  }
}
