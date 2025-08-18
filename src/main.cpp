#include "utils/pch.h"
#include "core/application.h"

std::shared_ptr<Application> application;

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
        application = std::make_shared<Application>(hInstance, config);
        return application->run();
    }
    catch (const std::exception &e)
    {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
