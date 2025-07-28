#include "Win32Window.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int Win32Window::winMain(
    const WindowConfig& config,
    Engine& engine,
    HINSTANCE hInstance,
    int nCmdShow
) {
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = config.windowClassName;
    windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&windowClass)) {
        MessageBox(0, L"RegisterClassEx failed!", 0, 0);
        return -1;
    }

    HWND hwnd = createWindow(
        config.windowClassName,
        hInstance,
        config.appName,
        config.width,
        config.height
    );

    if (!hwnd) {
        MessageBox(0, L"CreateWindowEx failed!", 0, 0);
        return -1;
    }

    ShowWindow(hwnd, nCmdShow);
    
    UpdateWindow(hwnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            engine.update();
        }
    }

    return static_cast<int>(msg.wParam);
}

HWND Win32Window::createWindow(
    LPCWSTR windowClassName, 
    HINSTANCE hInstance,
    LPCWSTR windowTitle, 
    uint32_t width, 
    uint32_t height
) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    RECT windowRect = {
        0,
        0,
        static_cast<LONG>(width), 
        static_cast<LONG>(height)
    };

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    HWND hwnd = CreateWindowEx(
        0,
        windowClassName,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        windowX, 
        windowY,
        windowWidth,
        windowHeight,
        nullptr, 
        nullptr,
        hInstance, 
        nullptr
    );

    return hwnd;
}
