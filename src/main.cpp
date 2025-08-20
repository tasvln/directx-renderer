#include "utils/pch.h"
#include "core/application.h"

// std::unique_ptr<Application> application;

WindowConfig config = {
    .appName = L"DirectX Graphic Renderer",
    .windowClassName = L"DirectWindowClass",
    .width = 1440,
    .height = 700,
    .enabledDirectX = false,
    .useWarp = false};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        auto application = std::make_unique<Application>(hInstance, config);
        return application->run();
    }
    catch (const std::exception &e)
    {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
