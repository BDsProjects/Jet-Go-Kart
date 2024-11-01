// main.cpp
#include <windows.h>
#include <d3d11.h>
#include <tchar.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <vector>
#include <cmath>
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>


// DirectX 11 objects
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Application state
enum class Page {
    MainMenu,
    TurbineOperation,
    GSUTestFunctions,
    LogDataScreen,
    ECUSettings
};

struct AppState {
    Page currentPage = Page::MainMenu;



    // Example state variables for different pages
    struct {
        float turbineSpeed = 0.0f;
        bool isRunning = false;
    } turbineState;

    struct {
        bool testInProgress = false;
        int testStage = 0;
    } gsuState;

    struct {
        std::vector<float> logData;
        bool isLogging = false;
    } logState;

    struct {
        float parameter1 = 0.0f;
        float parameter2 = 0.0f;
        bool setting1 = false;
    } ecuSettings;
};

AppState g_appState;

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include "imgui.h"

void RenderMainMenu() {
    // Window setup
    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

    ImGui::Begin("Control System Interface", nullptr, window_flags);

    // Calculate dimensions
    float buttonWidth = windowSize.x * 0.3f;  // 30% of window width
    float buttonHeight = windowSize.y * 0.12f; // 12% of window height
    float buttonSpacing = windowSize.y * 0.05f; // 5% of window height
    float windowWidth = ImGui::GetWindowWidth();
    float buttonX = (windowWidth - buttonWidth) * 0.5f;

    // Logo rendering section
    // Company logo
    if (g_appState.logoManager.IsLogoLoaded("company_logo")) {
        ImVec2 logoSize = g_appState.logoManager.GetLogoSize("company_logo");
        // Scale logo to fit width while maintaining aspect ratio
        float maxLogoWidth = windowWidth * 0.6f; // 60% of window width
        float scale = std::min(maxLogoWidth / logoSize.x, windowSize.y * 0.2f / logoSize.y);
        ImVec2 scaledSize(logoSize.x * scale, logoSize.y * scale);

        // Center the logo
        float logoX = (windowWidth - scaledSize.x) * 0.5f;
        ImGui::SetCursorPosX(logoX);
        ImGui::SetCursorPosY(windowSize.y * 0.05f); // 5% from top

        // Add a slight tint to match the button theme
        ImVec4 logoTint(0.9f, 0.9f, 1.0f, 1.0f);
        g_appState.logoManager.RenderLogo("company_logo", scaledSize, logoTint);
    }

    // Product logo (smaller, below company logo)
    if (g_appState.logoManager.IsLogoLoaded("product_logo")) {
        ImVec2 logoSize = g_appState.logoManager.GetLogoSize("product_logo");
        float maxLogoWidth = windowWidth * 0.4f; // 40% of window width
        float scale = std::min(maxLogoWidth / logoSize.x, windowSize.y * 0.15f / logoSize.y);
        ImVec2 scaledSize(logoSize.x * scale, logoSize.y * scale);

        float logoX = (windowWidth - scaledSize.x) * 0.5f;
        ImGui::SetCursorPosX(logoX);
        ImGui::SetCursorPosY(windowSize.y * 0.28f); // Position below company logo

        ImVec4 logoTint(0.85f, 0.85f, 0.95f, 1.0f);
        g_appState.logoManager.RenderLogo("product_logo", scaledSize, logoTint);
    }

    // Button section - start lower to accommodate logos
    ImGui::SetCursorPosY(windowSize.y * 0.45f); // Start buttons at 45% from top

    // Style the buttons with a darker theme
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.3f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.5f, 1.0f));

    // Add subtle text shadow to buttons
    ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
    auto RenderButtonWithShadow = [&](const char* label, const ImVec2& size) {
        ImGui::SetCursorPosX(buttonX);
        ImVec2 textPos = ImGui::GetCursorPos();

        // Render button
        bool clicked = ImGui::Button(label, size);

        // Render shadow text (disabled for better clarity)
        /*ImGui::SetCursorPos(ImVec2(textPos.x + 1, textPos.y + 1));
        ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.5f), label);*/

        return clicked;
        };

    // Render each button with proper spacing
    if (RenderButtonWithShadow("Turbine Operation", ImVec2(buttonWidth, buttonHeight)))
        g_appState.currentPage = Page::TurbineOperation;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    if (RenderButtonWithShadow("GSU Test Functions", ImVec2(buttonWidth, buttonHeight)))
        g_appState.currentPage = Page::GSUTestFunctions;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    if (RenderButtonWithShadow("Log Data Screen", ImVec2(buttonWidth, buttonHeight)))
        g_appState.currentPage = Page::LogDataScreen;

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    if (RenderButtonWithShadow("ECU Settings", ImVec2(buttonWidth, buttonHeight)))
        g_appState.currentPage = Page::ECUSettings;

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
    ImGui::End();
}

// Utility function to convert degrees to radians
constexpr float DEG2RAD = 0.0174532925f;

void DrawCircularGauge(const char* label, float value, float minValue, float maxValue,
    float radius = 40.0f, ImU32 color = IM_COL32(255, 0, 0, 255),
    const char* format = "%.0f", float angleMin = 135.0f, float angleMax = 405.0f) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 center = ImVec2(pos.x + radius, pos.y + radius);

    // Draw gauge background
    const float angleRange = angleMax - angleMin;
    const int numSegments = 32;
    draw_list->AddCircle(center, radius, IM_COL32(128, 128, 128, 255), numSegments, 2.0f);

    // Draw gauge arc
    float t = (value - minValue) / (maxValue - minValue);
    float angleValue = angleMin + t * angleRange;

    // Draw major ticks and labels
    const int numTicks = 10;
    for (int i = 0; i <= numTicks; i++) {
        float tick_angle = angleMin + (i * angleRange / numTicks);
        float cos_a = cosf(tick_angle * DEG2RAD);
        float sin_a = sinf(tick_angle * DEG2RAD);

        ImVec2 tick_pos = ImVec2(center.x + cos_a * radius,
            center.y + sin_a * radius);
        ImVec2 tick_pos2 = ImVec2(center.x + cos_a * (radius - 8),
            center.y + sin_a * (radius - 8));

        draw_list->AddLine(tick_pos, tick_pos2, IM_COL32(255, 255, 255, 255), 1.0f);

        // Add labels for major ticks
        float labelValue = minValue + (i * (maxValue - minValue) / numTicks);
        char label_text[32];
        sprintf_s(label_text, "%.0f", labelValue);

        ImVec2 label_pos = ImVec2(center.x + cos_a * (radius - 20),
            center.y + sin_a * (radius - 20));
        draw_list->AddText(label_pos, IM_COL32(255, 255, 255, 255), label_text);
    }

    // Draw needle
    float cos_a = cosf(angleValue * DEG2RAD);
    float sin_a = sinf(angleValue * DEG2RAD);
    ImVec2 needle_end = ImVec2(center.x + cos_a * (radius - 5),
        center.y + sin_a * (radius - 5));
    draw_list->AddLine(center, needle_end, color, 2.0f);

    // Draw center dot
    draw_list->AddCircleFilled(center, 5.0f, color);

    // Draw value text
    char overlay_text[32];
    sprintf_s(overlay_text, format, value);
    ImVec2 text_size = ImGui::CalcTextSize(overlay_text);
    ImVec2 text_pos = ImVec2(center.x - text_size.x * 0.5f,
        center.y + radius * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), overlay_text);

    // Draw label
    ImVec2 label_size = ImGui::CalcTextSize(label);
    ImVec2 label_pos = ImVec2(center.x - label_size.x * 0.5f,
        center.y - radius * 0.5f);
    draw_list->AddText(label_pos, IM_COL32(255, 255, 255, 255), label);

    // Reserve space for the gauge in ImGui
    ImGui::Dummy(ImVec2(radius * 2, radius * 2));
}

void RenderTurbineOperation()
{
    ImGui::Begin("Turbine Operation", nullptr, ImGuiWindowFlags_NoCollapse);
    if (ImGui::Button("Back to Main Menu"))
        g_appState.currentPage = Page::MainMenu;
    ImGui::Separator();

    // Add some spacing and create a row for gauges
    ImGui::BeginGroup();
    ImGui::Columns(2, "gauges", false);

    // RPM Gauge
    DrawCircularGauge("RPM",
        g_appState.turbineState.turbineSpeed * 60.0f, // Convert speed percentage to RPM
        0.0f,    // Min RPM
        6000.0f, // Max RPM
        50.0f,   // Radius
        IM_COL32(255, 0, 0, 255), // Red color
        "%.0f RPM");

    ImGui::NextColumn();

    // Temperature Gauge
    static float temperature = 25.0f; // You might want to add this to your turbine state
    DrawCircularGauge("Temperature",
        temperature,
        0.0f,    // Min temperature
        150.0f,  // Max temperature
        50.0f,   // Radius
        IM_COL32(255, 165, 0, 255), // Orange color
        "%.1f °C");

    ImGui::Columns(1);
    ImGui::EndGroup();

    ImGui::Separator();

    // Original controls
    ImGui::SliderFloat("Turbine Speed", &g_appState.turbineState.turbineSpeed, 0.0f, 100.0f, "%.1f%%");
    ImGui::Checkbox("Turbine Running", &g_appState.turbineState.isRunning);

    // Add temperature control slider
    ImGui::SliderFloat("Temperature", &temperature, 0.0f, 150.0f, "%.1f °C");

    ImGui::End();
}

void RenderGSUTestFunctions()
{
    ImGui::Begin("GSU Test Functions", nullptr, ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Back to Main Menu"))
        g_appState.currentPage = Page::MainMenu;

    ImGui::Separator();

    if (!g_appState.gsuState.testInProgress) {
        if (ImGui::Button("Start Test"))
            g_appState.gsuState.testInProgress = true;
    }
    else {
        if (ImGui::Button("Stop Test"))
            g_appState.gsuState.testInProgress = false;

        ImGui::ProgressBar(g_appState.gsuState.testStage / 100.0f);
    }

    ImGui::End();
}

void RenderLogDataScreen()
{
    ImGui::Begin("Log Data Screen", nullptr, ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Back to Main Menu"))
        g_appState.currentPage = Page::MainMenu;

    ImGui::Separator();

    if (ImGui::Button(g_appState.logState.isLogging ? "Stop Logging" : "Start Logging"))
        g_appState.logState.isLogging = !g_appState.logState.isLogging;

    if (!g_appState.logState.logData.empty()) {
        ImGui::PlotLines("Log Data", g_appState.logState.logData.data(),
            g_appState.logState.logData.size(), 0, nullptr,
            FLT_MAX, FLT_MAX, ImVec2(0, 80));
    }

    ImGui::End();
}

void RenderECUSettings()
{
    ImGui::Begin("ECU Settings", nullptr, ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Back to Main Menu"))
        g_appState.currentPage = Page::MainMenu;

    ImGui::Separator();

    ImGui::SliderFloat("Parameter 1", &g_appState.ecuSettings.parameter1, 0.0f, 100.0f, "%.1f");
    ImGui::SliderFloat("Parameter 2", &g_appState.ecuSettings.parameter2, 0.0f, 100.0f, "%.1f");
    ImGui::Checkbox("Setting 1", &g_appState.ecuSettings.setting1);

    if (ImGui::Button("Save Settings")) {
        // Add save settings functionality
    }

    if (ImGui::Button("Load Settings")) {
        // Add load settings functionality
    }

    ImGui::End();
}

// Main code
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"DX11 ImGui App", nullptr };
    RegisterClassExW(&wc);
    HWND hwnd = CreateWindowW(wc.lpszClassName, L"Control System Interface", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Setup style
    ImGui::StyleColorsDark();

    // Customize style for industrial application
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 4.0f;
    style.WindowRounding = 4.0f;
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.3f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.5f, 1.0f);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render current page
        switch (g_appState.currentPage)
        {
        case Page::MainMenu:
            RenderMainMenu();
            break;
        case Page::TurbineOperation:
            RenderTurbineOperation();
            break;
        case Page::GSUTestFunctions:
            RenderGSUTestFunctions();
            break;
        case Page::LogDataScreen:
            RenderLogDataScreen();
            break;
        case Page::ECUSettings:
            RenderECUSettings();
            break;
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { 0.1f, 0.1f, 0.1f, 1.00f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions implementation remains the same
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2,
        D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}