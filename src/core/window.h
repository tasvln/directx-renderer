#pragma once

#include "utils/pch.h"
#include "utils/event_interface.h"
#include "window_manger.h"

class Window
{
public:
    friend class WindowManager;

    Window(HINSTANCE hInstance, WindowConfig &config);
    ~Window();

    HWND getHwnd() const
    {
        return hwnd;
    }

    void show();
    void pumpMessages();
    void run();
    void destroy();

    void registerEventHandler(std::shared_ptr<IWindowEventHandler> handler)
    {
        eventHandler = handler;
    }

    void enableDebugLayer();

    bool checkForTearingSupport();

    HWND createWindow(HINSTANCE hInstance);

protected:
    void dispatchUpdate(UpdateEventArgs &e);
    void dispatchRender(RenderEventArgs &e);
    void dispatchKeyPressed(KeyEventArgs &e);
    void dispatchMouseWheel(KeyEventArgs &e);
    void dispatchResize(KeyEventArgs &e);
    void dispatchWindowDestroy(KeyEventArgs &e);

private:
    HWND hwnd = nullptr;
    static std::unordered_map<HWND, Window *> hwndMap;
    WindowConfig config;
    RECT windowRect = {};

    uint64_t frameCount;

    std::weak_ptr<IWindowEventHandler> eventHandler;

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};