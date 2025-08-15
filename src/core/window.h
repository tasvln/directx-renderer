#pragma once

#include "utils/pch.h"
#include "utils/event_interface.h" // IWindowEventHandler + Event args
#include <chrono>

class Window
{
public:
    Window(HINSTANCE hInstance, WindowConfig& config);
    ~Window();

    HWND getHwnd() const { 
        return hwnd; 
    }

    // Event wiring
    void registerEventHandler(std::shared_ptr<IWindowEventHandler> handler) { 
        eventHandler = handler; 
    }

    // Window actions
    void show();
    void pumpMessages();        // process OS messages only
    void destroy();
    void quit(int exitCode = 0);

    void setFullScreen(bool toggleFullscreen);
    void toggleFullscreen();

    void toggleVsync();
    void setVsync();
    
    HWND createWindow(HINSTANCE hInstance);

private:
    HWND hwnd = nullptr;
    WindowConfig config;
    RECT windowRect = {};
    std::weak_ptr<IWindowEventHandler> eventHandler;

    // Timing for update/render event args
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastTime;

    // Win32 callbacks
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT handleWndProc(UINT msg, WPARAM wParam, LPARAM lParam);

    // helpers
    void enableDebugLayer();
};
