#include "Win32Window.h"

Win32Window::Win32Window(
    WindowConfig& config, 
    HINSTANCE hInst, 
    int nCmdShow
)   : 
    hInstance(hInst),
    config(config)
{
    hwnd = createWindow(nCmdShow);
    LOG_INFO(L"createWindow(nCmdShow)");

    if (!hwnd) {
        DWORD err = GetLastError();  // Capture error immediately
        wchar_t buf[256];
        wsprintf(buf, L"CreateWindowEx failed! Error code: %lu", err);
        MessageBox(0, buf, L"Win32 Error", 0);
        throw std::runtime_error("CreateWindowEx failed");
    }
}

int Win32Window::run(Engine& engine) {
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&engine));

    ShowWindow(hwnd, SW_SHOW);
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

HWND Win32Window::createWindow(int nCmdShow) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    windowRect = {
        0,
        0,
        static_cast<LONG>(config.width),
        static_cast<LONG>(config.height)
    };
    LOG_INFO(L"windowRect set");

    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

    int windowWidth = windowRect.right - windowRect.left;
    int windowHeight = windowRect.bottom - windowRect.top;

    int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
    int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);

    LOG_INFO(L"windowX & windowY");

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

void Win32Window::setEngine(Engine* enginePtr) {
    engine = enginePtr;
    
    LOG_INFO(L"Engine Set in Window Class");
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Win32Window* window = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        window = reinterpret_cast<Win32Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
        return DefWindowProc(hwnd, msg, wParam, lParam);  // Return default processing result
    }

    if (window && window->config.enabledDirectX) {
        Engine* engine = window->engine;

        switch (msg) {
            case WM_PAINT:
                engine->update();
                engine->render();
                return 0;

            case WM_SYSKEYDOWN:
            case WM_KEYDOWN: {
                bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
                switch (wParam) {
                    case 'V':
                        window->config.vsync = !window->config.vsync;
                        break;
                    case VK_ESCAPE:
                        ::PostQuitMessage(0);
                        break;
                    case VK_RETURN:
                    case VK_F11:
                        if (alt) {
                            engine->setFullScreen(!window->config.fullscreen);
                        }
                        break;
                }
                return 0;
            }

            case WM_SIZE: {
                RECT clientRect = {};
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;
                engine->resize(width, height);
                return 0;
            }

            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    } else {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
