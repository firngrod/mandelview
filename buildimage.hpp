#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include "mandelbrot.hpp"
void BuildImage(cv::Mat &image, const MandelbrotView &theView, const Json::Value &colDefs);
