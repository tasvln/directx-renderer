#include "window.h"
#include "application.h"
#include "utils/events.h"

Window::Window(
    HINSTANCE hInstance, 
    WindowConfig& config, 
    Application* app
) :
    config(config),
    app(app)
{
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = wndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = config.windowClassName;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Unable to register the window class.", L"Error", MB_OK | MB_ICONERROR);
        LOG_ERROR(L"RegisterClassEx failed!");
    }
    LOG_INFO(L"RegisterClassEx created!");

    hwnd = createWindow(hInstance);

    if (!hwnd) {
        LOG_ERROR(L"Window handle is null!");
        throw std::runtime_error("Failed to create window");
    }

    ShowWindow(hwnd, SW_SHOW);
}

Window::~Window() {
    if (hwnd) {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

HWND Window::createWindow(HINSTANCE hInstance)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    windowRect = {
        0,
        0,
        static_cast<LONG>(config.width),
        static_cast<LONG>(config.height)
    };

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    return CreateWindowEx(
        0,
        config.windowClassName,
        config.appName,
        WS_OVERLAPPEDWINDOW,
        windowX,
        windowY,
        windowWidth,
        windowHeight,
        nullptr,
        nullptr,
        hInstance,
        this
    );
}

LRESULT CALLBACK Window::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(cs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd = hWnd;
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE:
            if (app)
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);

                ResizeEventArgs resizeArgs(width, height);
                app->onResize(resizeArgs);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}