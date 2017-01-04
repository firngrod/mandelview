#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/core/mat.hpp>
#include <json/json.h>
#include "mandelbrot.hpp"

void BuildImage(MandelbrotView &theView, Json::Value colDefs, uint64_t * highItr = NULL);
void SetRectangle(MandelbrotView &theView);
