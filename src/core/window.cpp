#include "window.h"

Window::Window(
    HINSTANCE hInstance,
    WindowConfig &config) : config(config)
{
    // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
    // Using this awareness context allows the client area of the window
    // to achieve 100% scaling while still allowing non-client window content to
    // be rendered in a DPI sensitive fashion.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    enableDebugLayer();

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
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
        LOG_INFO(L"RegisterClassEx failed!");
    }
    LOG_INFO(L"RegisterClassEx created!");

    hwnd = createWindow(hInstance);
    hwndMap[hwnd] = this;
}

Window::~Window()
{
    hwndMap.erase(hwnd);
    DestroyWindow(hwnd);
}

void Window::enableDebugLayer()
{
#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;

    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    }
    else
    {
        LOG_ERROR(L"Failed to enable debug layer!");
    }
#endif
}

HWND Window::createWindow(HINSTANCE hInstance)
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    windowRect = {
        0,
        0,
        static_cast<LONG>(config.width),
        static_cast<LONG>(config.height)};

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
        this);
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window *window = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT *cs = reinterpret_cast<CREATESTRUCT *>(lParam);
        window = static_cast<Window *>(cs->lpCreateParams);
        hwndMap[hwnd] = window;
        window->hwnd = hwnd;
    }
    else
    {
        auto it = hwndMap.find(hwnd);
        if (it != hwndMap.end())
            window = it->second;
    }

    if (window)
    {
        return window->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}