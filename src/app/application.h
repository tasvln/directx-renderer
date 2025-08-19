#pragma once

#include "utils/pch.h"
#include "engine/device.h"

class Window;

class Application
{
    public:
        Application(HINSTANCE hInstance, WindowConfig &config);
        ~Application();

        // running the application
        int run();

        void onResize(UINT width, UINT height);

        // main functions for rendering
        void onUpdate();
        void onRender();

    private:
        void init();

    private:
        HWND hwnd = nullptr;
        WindowConfig config;
        RECT windowRect = {};

        // unique pttrsssssss -> GPU resources
        std::unique_ptr<Window> window;
        // std::unique_ptr<Device> device;
};