#include "core/pch.h"
#include "core/Win32Window.h"
#include "engine/engine.h"

// #define HInstance() GetModuleHandle(nullptr);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();
    
    WindowConfig config;
    config.appName = L"DirectX Graphic Renderer";
    config.windowClassName = L"DirectWindowClass";
    config.width = 1440;
    config.height = 700;

    engine->init();

    return Win32Window::winMain(config, *engine, hInstance, nCmdShow);
}
