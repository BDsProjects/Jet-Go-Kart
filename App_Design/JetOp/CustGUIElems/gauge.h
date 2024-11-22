#ifndef GAUGE_H
#define GAUGE_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

void DrawCircularGauge(const char* label, float value, float minValue, float maxValue, float radius = 100.0f);

#endif // GAUGE_H