// main.cpp
#include <windows.h>
#include <d3d11.h>
#include <tchar.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <vector>

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

// Get the primary monitor's screen dimensions
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);

// Page rendering functions
void RenderMainMenu()
{   

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Main Menu", nullptr, 
        ImGuiWindowFlags_NoResize |      // Prevent user resizing
        ImGuiWindowFlags_NoMove |        // Prevent window moving
        ImGuiWindowFlags_NoCollapse |    // Prevent window collapsing
        ImGuiWindowFlags_NoTitleBar);    // Remove title bar);
        

    if (ImGui::Button("Turbine Operation", ImVec2(200, 50)))
        g_appState.currentPage = Page::TurbineOperation;

    if (ImGui::Button("GSU Test Functions", ImVec2(200, 50)))
        g_appState.currentPage = Page::GSUTestFunctions;

    if (ImGui::Button("Log Data Screen", ImVec2(200, 50)))
        g_appState.currentPage = Page::LogDataScreen;

    if (ImGui::Button("ECU Settings", ImVec2(200, 50)))
        g_appState.currentPage = Page::ECUSettings;

    if (ImGui::Button("Exit JetOp", ImVec2(200, 50))) {
        std::exit(0); // An abrupt way to exit the application
    }
    ImGui::End();
}

void RenderTurbineOperation()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Turbine Operation", nullptr,
        ImGuiWindowFlags_NoResize |      // Prevent user resizing
        ImGuiWindowFlags_NoMove |        // Prevent window moving
        ImGuiWindowFlags_NoCollapse |    // Prevent window collapsing
        ImGuiWindowFlags_NoTitleBar);    // Remove title bar);

    

    if (ImGui::Button("Back to Main Menu"))
        g_appState.currentPage = Page::MainMenu;

    ImGui::Separator();

    ImGui::SliderFloat("Turbine Speed", &g_appState.turbineState.turbineSpeed, 0.0f, 100.0f, "%.1f%%");
    ImGui::Checkbox("Turbine Running", &g_appState.turbineState.isRunning);

    // Add more turbine controls here

    ImGui::End();
}

void RenderGSUTestFunctions()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("GSU Test Functions", nullptr,
        ImGuiWindowFlags_NoResize |      // Prevent user resizing
        ImGuiWindowFlags_NoMove |        // Prevent window moving
        ImGuiWindowFlags_NoCollapse |    // Prevent window collapsing
        ImGuiWindowFlags_NoTitleBar);    // Remove title bar);

    

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

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Log Data Screen", nullptr,
        ImGuiWindowFlags_NoResize |      // Prevent user resizing
        ImGuiWindowFlags_NoMove |        // Prevent window moving
        ImGuiWindowFlags_NoCollapse |    // Prevent window collapsing
        ImGuiWindowFlags_NoTitleBar);    // Remove title bar);

    

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
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("ECU Settings", nullptr,
        ImGuiWindowFlags_NoResize |      // Prevent user resizing
        ImGuiWindowFlags_NoMove |        // Prevent window moving
        ImGuiWindowFlags_NoCollapse |    // Prevent window collapsing
        ImGuiWindowFlags_NoTitleBar);    // Remove title bar);

    

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
    // Get the primary monitor's screen dimensions
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // Create application window
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"DX11 ImGui App", nullptr };
    RegisterClassExW(&wc);

    // Create a Borderless fullscreen window
    HWND hwnd = CreateWindowW(wc.lpszClassName, L"Control System Interface", WS_POPUP | WS_SYSMENU, 0, 0, screenWidth, screenHeight, nullptr, nullptr, wc.hInstance, nullptr);
    //                        window class name, window title, window style (borderless), X-pos top-left-corner, Y-pos top-left-corner, width and height of the window, parent window, Menu, Instance handle, additional app data
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    //// Optional: Set ImGui to use full screen dimensions
    //ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));
    //ImGui::SetNextWindowPos(ImVec2(0, 0));

    // Show the window
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Disable window resizing in ImGui
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigWindowsResizeFromEdges = false; // Prevent resize from edges

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