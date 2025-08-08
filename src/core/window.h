#pragma once

#include "utils/pch.h"
#include "utils/event_interface.h"

class Window {
    public:
        Window(HINSTANCE hInstance, WindowConfig& config);
        ~Window();

        HWND getHwnd() const {
            return hwnd;
        }

        void show();
        void pumpMessages();
        void run();

        void registerEventHandler(std::shared_ptr<IWindowEventHandler> handler) {
            eventHandler = handler;
        }

        void enableDebugLayer();

        HWND createWindow(HINSTANCE hInstance);

    protected:
        void dispatchUpdate(UpdateEventArgs& e);
        void dispatchRender(RenderEventArgs& e);
        void dispatchKeyPressed(KeyEventArgs& e);
        void dispatchKeyReleased(KeyEventArgs& e);
        void dispatchMouseMoved(KeyEventArgs& e);
        void dispatchMouseButtonPressed(KeyEventArgs& e);
        void dispatchMouseButtonReleased(KeyEventArgs& e);
        void dispatchMouseWheel(KeyEventArgs& e);
        void dispatchResize(KeyEventArgs& e);
        void dispatchWindowDestroy(KeyEventArgs& e);

    private:
        HWND hwnd = nullptr;
        WindowConfig config;
        RECT windowRect = {};

        uint64_t frameCount;

        std::weak_ptr<IWindowEventHandler> eventHandler;

        static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
        LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
};