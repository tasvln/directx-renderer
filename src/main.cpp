#include "core/pch/pch.h"


// #define HInstance() GetModuleHandle(nullptr);

INT windowWidth;
INT windowHeight;

LPCWSTR appName;
LPCWSTR windowClassName;

void initVariables() {
    appName = L"DirectX Graphic Renderer";
    windowClassName = L"DirectWindowClass";

    windowWidth = 1440;
    windowHeight = 768;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    initVariables();
    
    // Create Window Class
    WNDCLASSEXW windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;

    windowClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    windowClass.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    windowClass.lpszClassName = windowClassName;
    windowClass.lpszMenuName = nullptr;

    windowClass.hInstance = hInstance;

    windowClass.lpfnWndProc = WndProc;

    RegisterClassEx(&windowClass);

    // Create and Display Window

    // RECT windowRect = { 0, 0, static_cast<LONG>(pSample->GetWidth()), static_cast<LONG>(pSample->GetHeight()) };
    // AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowEx(
        0,
        windowClassName, // class name must match windowClass.lpszClassName
        appName,            // window title
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,       // x
        0,       // y
        windowWidth,         // width
        windowHeight,        // height
        nullptr,             // parent window
        nullptr,             // menu
        hInstance,           // instance handle
        nullptr              // additional application data
    );

    if (!hwnd) {
        MessageBox(0, L"Window creation failed!", 0, 0);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Listen for events (Message Loop)
    MSG msg = { 0 };

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
