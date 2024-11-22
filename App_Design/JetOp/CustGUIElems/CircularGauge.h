// CircularGauge.h
#ifndef CIRCULAR_GAUGE_H
#define CIRCULAR_GAUGE_H

#include <imgui.h>
#include <cmath>

void DrawCircularGauge(const char* label, float value, float minValue, float maxValue,
    float radius = 40.0f, ImU32 color = IM_COL32(0, 255, 0, 255),
    const char* format = "%.0f", float angleMin = 135.0f, float angleMax = 405.0f);

#endif // CIRCULAR_GAUGE_H
