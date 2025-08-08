#include "utils/pch.h"
#include "engine/engine.h"
#include "core/window.h"

// #define HInstance() GetModuleHandle(nullptr);

std::unique_ptr<Engine> engine = nullptr;
std::unique_ptr<Window> window = nullptr;

WindowConfig config;

HWND hwnd = nullptr;

RECT windowRect = {};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // create config
    config.appName = L"DirectX Graphic Renderer";
    config.windowClassName = L"DirectWindowClass";
    config.width = 1440;
    config.height = 700;
    config.enabledDirectX = false;
    config.useWarp = false;

    // create window
    window = std::make_unique<Window>(
        hInstance,
        config
    );

    try {


        return 0;
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
