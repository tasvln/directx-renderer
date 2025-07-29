#include "utils/pch.h"
#include "core/Win32Window.h"
#include "engine/engine.h"

// #define HInstance() GetModuleHandle(nullptr);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WindowConfig config;
    config.appName = L"DirectX Graphic Renderer";
    config.windowClassName = L"DirectWindowClass";
    config.width = 1440;
    config.height = 700;
    config.enabledDirectX = false;
    config.useWarp = false;
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Win32Window::WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = config.windowClassName;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc)) {
        MessageBox(0, L"RegisterClassEx failed!", 0, 0);
    }

    try {
        Win32Window window(config, hInstance, nCmdShow);

        HWND hwnd = window.getHwnd();
        RECT rect = window.getWindowRect();

        std::unique_ptr<Engine> engine = std::make_unique<Engine>(
            hwnd,
            config,
            rect
        );

        engine->init();

        window.setEngine(engine.get());
        
        return window.run(*engine);
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
