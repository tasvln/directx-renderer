#pragma once

#include "utils/pch.h"
#include "engine/engine.h"

class Win32Window {
    public:
        static int winMain(
            const WindowConfig& config, 
            Engine& engine,
            HINSTANCE hInstance, 
            int nCmdShow
        );
        
        static HWND createWindow(
            const wchar_t* windowClassName, 
            HINSTANCE hInst,
            const wchar_t* windowTitle, 
            uint32_t width, 
            uint32_t height
        );
    private:
};