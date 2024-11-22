#include "gauge.h"

void DrawCircularGauge(const char* label, float value, float minValue, float maxValue, float radius) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Get the current cursor position for the gauge center
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2(pos.x + radius, pos.y + radius);

    // Calculate normalized value (0.0 to 1.0)
    float normalizedValue = (value - minValue) / (maxValue - minValue);
    normalizedValue = ImClamp(normalizedValue, 0.0f, 1.0f);

    // Colors
    ImU32 bgColor = IM_COL32(50, 50, 50, 255);        // Dark gray background
    ImU32 fillColor = IM_COL32(66, 150, 250, 255);    // Blue fill
    ImU32 borderColor = IM_COL32(255, 255, 255, 255); // White border

    // Draw parameters
    float thickness = 10.0f;
    float innerRadius = radius - thickness - 5.0f;

    // Background circle
    draw_list->AddCircle(center, radius, bgColor, 32, thickness);

    // Calculate start and end angles (clockwise from -140 to +140 degrees)
    float startAngle = -140.0f * (M_PI / 180.0f);
    float endAngle = 140.0f * (M_PI / 180.0f);
    float angleRange = endAngle - startAngle;
    float valueAngle = startAngle + (angleRange * normalizedValue);

    // Draw the filled arc
    int numSegments = 32;
    draw_list->PathArcTo(center, radius, startAngle, valueAngle, numSegments);
    draw_list->PathStroke(fillColor, 0, thickness);

    // Draw tick marks and labels
    int numTicks = 11; // 0 to 100 in steps of 10
    for (int i = 0; i < numTicks; i++) {
        float tick = i / (float)(numTicks - 1);
        float angle = startAngle + (angleRange * tick);

        // Calculate tick mark positions
        float cosA = cosf(angle);
        float sinA = sinf(angle);
        ImVec2 tickStart = ImVec2(
            center.x + cosA * (radius - thickness - 10),
            center.y + sinA * (radius - thickness - 10)
        );
        ImVec2 tickEnd = ImVec2(
            center.x + cosA * (radius - thickness + 5),
            center.y + sinA * (radius - thickness + 5)
        );

        // Draw tick mark
        draw_list->AddLine(tickStart, tickEnd, borderColor, 2.0f);

        // Add labels
        char label[32];
        snprintf(label, sizeof(label), "%.0f", minValue + (tick * (maxValue - minValue)));
        ImVec2 textPos = ImVec2(
            center.x + cosA * (radius - thickness - 25),
            center.y + sinA * (radius - thickness - 25)
        );
        draw_list->AddText(textPos, borderColor, label);
    }

    // Draw value in center
    char valueText[32];
    snprintf(valueText, sizeof(valueText), "%.1f%%", value);
    ImVec2 textSize = ImGui::CalcTextSize(valueText);
    draw_list->AddText(
        ImVec2(center.x - textSize.x * 0.5f, center.y - textSize.y * 0.5f),
        borderColor,
        valueText
    );

    // Reserve space in ImGui for the gauge
    ImGui::Dummy(ImVec2(radius * 2, radius * 2));
}