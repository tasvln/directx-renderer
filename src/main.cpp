#include "utils/pch.h"
#include "engine/engine.h"

// #define HInstance() GetModuleHandle(nullptr);

std::unique_ptr<Engine> engine = nullptr;

WindowConfig config;

HWND hwnd = nullptr;

RECT windowRect = {};

HWND createWindow(int nCmdShow, HINSTANCE hInstance) {
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
        nullptr
    );
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (engine && config.enabledDirectX) {
        switch (msg) {
            case WM_PAINT:
                // ValidateRect(hwnd, nullptr);
                engine->update();
                engine->render();
                break;
            case WM_SYSKEYDOWN:
            case WM_KEYDOWN: {
                bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
                switch (wParam) {
                    case 'V':
                        config.vsync = !config.vsync;
                        break;
                    case VK_ESCAPE:
                        ::PostQuitMessage(0);
                        break;
                    case VK_RETURN:
                        if (alt) {
                            case VK_F11:
                                engine->setFullScreen(!config.fullscreen);
                        }
                        break;
                }
            }
            break;
            // The default window procedure will play a system notification sound 
            // when pressing the Alt+Enter keyboard combination if this message is 
            // not handled.
            case WM_SYSCHAR:
            break;
            case WM_SIZE: {
                RECT clientRect {};
                GetClientRect(hwnd, &clientRect);

                int width = clientRect.right - clientRect.left;
                int height = clientRect.bottom - clientRect.top;

                engine->resize(width, height);
            }
            break;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
            default:
                return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
        
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
    // Using this awareness context allows the client area of the window 
    // to achieve 100% scaling while still allowing non-client window content to 
    // be rendered in a DPI sensitive fashion.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    config.appName = L"DirectX Graphic Renderer";
    config.windowClassName = L"DirectWindowClass";
    config.width = 1440;
    config.height = 700;
    config.enabledDirectX = false;
    config.useWarp = false;

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
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
        LOG_INFO(L"RegisterClassEx failed!");
    }
    LOG_INFO(L"RegisterClassEx created!");

    hwnd = createWindow(nCmdShow, hInstance);
    LOG_INFO(L"createWindow(nCmdShow)");

    if (!hwnd) {
        DWORD err = GetLastError();
        wchar_t buf[256];
        wsprintf(buf, L"CreateWindowEx failed! Error code: %lu", err);
        MessageBox(0, buf, L"Win32 Error", 0);
        return -1;  // Return error code instead of throwing
    }

    try {
        engine = std::make_unique<Engine>(
            hwnd,
            config,
            windowRect
        );

        LOG_INFO(L"std::unique_ptr<Engine> engine");

        engine->init();
        
        LOG_INFO(L"ShowWindow(hwnd, SW_SHOW); before");
        ShowWindow(hwnd, SW_SHOW);
        LOG_INFO(L"ShowWindow(hwnd, SW_SHOW);");
        UpdateWindow(hwnd);
        LOG_INFO(L"SUpdateWindow(hwnd);");

        if (!hwnd) {
            LOG_ERROR(L"Window handle is null!");
            MessageBox(nullptr, L"Window handle is null!", L"Error", MB_ICONERROR);
            return -1;
        }

        LOG_INFO(L"Entering message loop");
        MSG msg {};
        while (msg.message != WM_QUIT) {
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                LOG_INFO(L"PeekMessage: msg = 0x%x", msg.message); // Add this
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            } 
            // else {
            //     LOG_INFO(L"Calling engine.update()");
            //     engine->update();
            //     LOG_INFO(L"Calling engine.render()");
            //     engine->render();  // If this fails silently, it may be the cause
            // }
        }
        LOG_INFO(L"engine.flush(), engine ptr: %p", &engine);

        engine->flush();

        return 0;
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_ICONERROR);
        return -1;
    }
}
