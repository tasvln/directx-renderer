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
};