#pragma once

#include "utils/pch.h"

class Window;
class Device;
class CommandQueue;
class Swapchain;
class Mesh;

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
        UINT currentBackBufferIndex;

        D3D12_VIEWPORT viewport;
        D3D12_RECT scissorRect;

        // unique pttrsssssss -> GPU resources
        std::unique_ptr<Window> window;
        std::unique_ptr<Device> device;
        std::unique_ptr<CommandQueue> directCommandQueue;
        // std::unique_ptr<CommandQueue> computeCommandQueue;
        // std::unique_ptr<CommandQueue> copyCommandQueue;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<Mesh> mesh;
};