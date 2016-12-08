#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include "mandelbrot.hpp"

void BuildImage(MandelbrotView &theView, const Json::Value &colDefs);
void SetRectangle(MandelbrotView &theView);
