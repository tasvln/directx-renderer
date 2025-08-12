#include "window.h"

static const double SecondsFromDuration(const std::chrono::steady_clock::duration& d)
{
    return std::chrono::duration_cast<std::chrono::duration<double>>(d).count();
}

Window::Window(
    HINSTANCE hInstance,
    WindowConfig &config
) : 
    config(config)
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

    startTime = lastTime = std::chrono::steady_clock::now();
}

Window::~Window()
{
    destroy();
}

void Window::show()
{
    if (hwnd)
    {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

void Window::pumpMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int Window::run()
{
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // idle: fire update + render via event handler
            auto now = std::chrono::steady_clock::now();
            double elapsed = SecondsFromDuration(now - lastTime);
            double total = SecondsFromDuration(now - startTime);
            lastTime = now;

            if (auto handler = eventHandler.lock())
            {
                UpdateEventArgs uargs(elapsed, total);
                handler->onUpdate(uargs);

                RenderEventArgs rargs(elapsed, total);
                handler->onRender(rargs);
            }
        }
    }

    return static_cast<int>(msg.wParam);
}

void Window::destroy()
{
    if (hwnd)
    {
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

void Window::quit(int exitCode)
{
    PostQuitMessage(exitCode);
}

void Window::enableDebugLayer()
{
#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
    } else
    {
        LOG_ERROR(L"Failed to enable debug layer!");
    }
#endif
}

void Window::setFullScreen(bool toggleFullscreen) {
    if (config.fullscreen != toggleFullscreen) {
        config.fullscreen = toggleFullscreen;

        if (config.fullscreen) {
            // Store the current window dimensions so they can be restored 
            // when switching out of fullscreen state.
            GetWindowRect(hwnd, &windowRect);

            // Set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
 
            SetWindowLongW(hwnd, GWL_STYLE, windowStyle);

            // Query the name of the nearest display device for the window.
            // This is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo(hMonitor, &monitorInfo);

            SetWindowPos(
                hwnd, 
                HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE
            );

            ShowWindow(hwnd, SW_MAXIMIZE);
        } else {
            // Restore previous window style and position
            SetWindowLongW(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

            SetWindowPos(hwnd, HWND_NOTOPMOST,
                windowRect.left,
                windowRect.top,
                windowRect.right - windowRect.left,
                windowRect.bottom - windowRect.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE
            );

            ShowWindow(hwnd, SW_NORMAL);
        }
    }
}

void Window::toggleFullscreen() {
    setFullScreen(!config.fullscreen);
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
        this
    );
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Window* window = nullptr;

    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Window*>(cs->lpCreateParams);
        // store pointer on window user data so we can find it later
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        window->hwnd = hwnd;
    }
    else
    {
        window = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (window)
    {
        return window->handleWndProc(msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT Window::handleWndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // cache handler pointer; if expired, handler will be null
    auto handler = eventHandler.lock();

    switch (msg)
    {
    case WM_PAINT:
    {
        // compute times
        auto now = std::chrono::steady_clock::now();
        double elapsed = SecondsFromDuration(now - lastTime);
        double total   = SecondsFromDuration(now - startTime);
        lastTime = now;

        if (handler)
        {
            UpdateEventArgs uargs(elapsed, total);
            handler->onUpdate(uargs);

            RenderEventArgs rargs(elapsed, total);
            handler->onRender(rargs);
        }

        ValidateRect(hwnd, nullptr);
        return 0;
    }

    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
    {
        unsigned int c = 0;
        MSG peekMsg;
        if (PeekMessage(&peekMsg, hwnd, WM_CHAR, WM_CHAR, PM_NOREMOVE) && peekMsg.message == WM_CHAR)
        {
            // consume the WM_CHAR and get the character code
            GetMessage(&peekMsg, hwnd, WM_CHAR, WM_CHAR);
            c = static_cast<unsigned int>(peekMsg.wParam);
        }

        bool shift   = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt     = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

        KeyCode::Key key = static_cast<KeyCode::Key>(wParam);

        if (handler)
        {
            KeyEventArgs kargs(key, c, KeyEventArgs::Pressed, control, shift, alt);
            handler->onKeyPressed(kargs);
        }
        return 0;
    }

    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        bool shift   = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt     = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

        KeyCode::Key key = static_cast<KeyCode::Key>(wParam);
        unsigned int c = 0;
        unsigned int scanCode = (lParam & 0x00FF0000) >> 16;

        // Try to translate to unicode character
        unsigned char keyboardState[256];
        GetKeyboardState(keyboardState);
        wchar_t translated[4] = {};
        int result = ToUnicodeEx(static_cast<UINT>(wParam), scanCode, keyboardState, translated, _countof(translated), 0, nullptr);
        if (result > 0)
            c = translated[0];

        if (handler)
        {
            KeyEventArgs kargs(key, c, KeyEventArgs::Released, control, shift, alt);
            handler->onKeyReleased(kargs);
        }
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift   = (wParam & MK_SHIFT)  != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = static_cast<int>(SHORT(LOWORD(lParam)));
        int y = static_cast<int>(SHORT(HIWORD(lParam)));

        if (handler)
        {
            MouseMotionEventArgs margs(lButton, mButton, rButton, control, shift, x, y);
            handler->onMouseMoved(margs);
        }
        return 0;
    }

    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift   = (wParam & MK_SHIFT)  != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = static_cast<int>(SHORT(LOWORD(lParam)));
        int y = static_cast<int>(SHORT(HIWORD(lParam)));

        MouseButtonEventArgs::MouseButton btn = MouseButtonEventArgs::None;
        if (msg == WM_LBUTTONDOWN) btn = MouseButtonEventArgs::Left;
        else if (msg == WM_RBUTTONDOWN) btn = MouseButtonEventArgs::Right;
        else if (msg == WM_MBUTTONDOWN) btn = MouseButtonEventArgs::Middle;

        if (handler)
        {
            MouseButtonEventArgs bargs(btn, MouseButtonEventArgs::Pressed, lButton, mButton, rButton, control, shift, x, y);
            handler->onMouseButtonPressed(bargs);
        }
        return 0;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        bool lButton = (wParam & MK_LBUTTON) != 0;
        bool rButton = (wParam & MK_RBUTTON) != 0;
        bool mButton = (wParam & MK_MBUTTON) != 0;
        bool shift   = (wParam & MK_SHIFT)  != 0;
        bool control = (wParam & MK_CONTROL) != 0;

        int x = static_cast<int>(SHORT(LOWORD(lParam)));
        int y = static_cast<int>(SHORT(HIWORD(lParam)));

        MouseButtonEventArgs::MouseButton btn = MouseButtonEventArgs::None;
        if (msg == WM_LBUTTONUP) btn = MouseButtonEventArgs::Left;
        else if (msg == WM_RBUTTONUP) btn = MouseButtonEventArgs::Right;
        else if (msg == WM_MBUTTONUP) btn = MouseButtonEventArgs::Middle;

        if (handler)
        {
            MouseButtonEventArgs bargs(btn, MouseButtonEventArgs::Released, lButton, mButton, rButton, control, shift, x, y);
            handler->onMouseButtonReleased(bargs);
        }
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        // zDelta (number of notches)
        float zDelta = static_cast<float>(static_cast<int16_t>(HIWORD(wParam))) / static_cast<float>(WHEEL_DELTA);
        short keyStates = static_cast<short>(LOWORD(wParam));

        bool lButton = (keyStates & MK_LBUTTON) != 0;
        bool rButton = (keyStates & MK_RBUTTON) != 0;
        bool mButton = (keyStates & MK_MBUTTON) != 0;
        bool shift   = (keyStates & MK_SHIFT) != 0;
        bool control = (keyStates & MK_CONTROL) != 0;

        int x = static_cast<int>(SHORT(LOWORD(lParam)));
        int y = static_cast<int>(SHORT(HIWORD(lParam)));

        // convert screen -> client coords
        POINT pt{ x, y };
        ScreenToClient(hwnd, &pt);

        if (handler)
        {
            MouseWheelEventArgs wargs(zDelta, lButton, mButton, rButton, control, shift, static_cast<int>(pt.x), static_cast<int>(pt.y));
            handler->onMouseWheel(wargs);
        }
        return 0;
    }

    case WM_SIZE:
    {
        int width  = static_cast<int>(SHORT(LOWORD(lParam)));
        int height = static_cast<int>(SHORT(HIWORD(lParam)));

        if (handler)
        {
            ResizeEventArgs rargs(width, height);
            handler->onResize(rargs);
        }
        return 0;
    }

    case WM_DESTROY:
    {
        if (handler)
        {
            // notify window destroy if desired
            ResizeEventArgs dummy(0,0); // (you can create a proper WindowDestroyEvent if you want)
            // handler->onWindowDestroy(); // if present
        }

        // quit the process (single window app)
        PostQuitMessage(0);
        return 0;
    }

    default:
        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}