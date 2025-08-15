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
    int code = 0;
    // create window

    application = std::make_shared<Application>(hInstance, config);

    try
    {

        return 0;
    }
    catch (const std::exception &e)
    {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
