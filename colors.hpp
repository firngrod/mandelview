#pragma once
#include <vector>
#include <json/json.h>

struct Color
{
  unsigned char val[3];
  Color(const unsigned int &input)
  {
    val[0] = (input >> 0) & 0xFF;
    val[1] = (input >> 8) & 0xFF;
    val[2] = (input >> 16) & 0xFF;
  }
  Color(const int &first, const int &second, const double &weight)
  {
    val[0] = (unsigned char)(((double)((first >> 0) & 0xFF) * (1 - weight)) + ((double)((second >> 0) & 0xFF) * weight));
    val[1] = (unsigned char)(((double)((first >> 8) & 0xFF) * (1 - weight)) + ((double)((second >> 8) & 0xFF) * weight));
    val[2] = (unsigned char)(((double)((first >> 16) & 0xFF) * (1 - weight)) + ((double)((second >> 16) & 0xFF) * weight));
  }
  Color()
  {}
};

void CalculateColors(std::vector<Color> &colors, const Json::Value &colDefs);
