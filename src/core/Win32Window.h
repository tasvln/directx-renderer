#pragma once

#include "utils/pch.h"
#include "engine/engine.h"

class Win32Window {
    public:
        Win32Window(
            WindowConfig& config,
            HINSTANCE hInstance, 
            int nCmdShow
        );
        ~Win32Window() = default;

        int run(
            Engine& engine
        );

        HWND getHwnd() const {
            return hwnd;
        }
        
        RECT getWindowRect() const {
            return windowRect;
        }

        void setEngine(Engine* enginePtr);

        
        static LRESULT CALLBACK WndProc(
            HWND hwnd, 
            UINT msg, 
            WPARAM wParam, 
            LPARAM lParam
        );
    private:
        WindowConfig& config;
        Engine* engine = nullptr;
        HWND hwnd = nullptr;
        RECT windowRect = {};
        HINSTANCE hInstance = nullptr;
        
        HWND createWindow(
            int nCmdShow
        );
};