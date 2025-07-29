#pragma once

#include "utils/pch.h"
#include "utils/frame_timer.h"

#include "device.h"
#include "command_queue.h"
#include "swapchain.h"
#include "command_allocator.h"
#include "command_list.h"
#include "fence.h"

class Engine {
    public:
        Engine(
            HWND& hwnd,
            WindowConfig& config,
            RECT& windowRect
        );
        
        ~Engine();

        void init();

        void update();

        void render();

        void resize(uint32_t width, uint32_t height);

        void setFullScreen(bool toggleFullscreen);

    private:
        HWND& hwnd;
        WindowConfig& config;
        RECT& windowRect;

        UINT currentIndex{};
        UINT rtvDescriptorSize{};

        uint64_t frameFenceCounts[FRAMEBUFFERCOUNT]{};
        BOOL isTearingSupported = false;

        FrameTimer timer;

        ComPtr<ID3D12Resource> backBuffers[FRAMEBUFFERCOUNT];
        std::vector<std::unique_ptr<CommandAllocator>> commandAllocators{};
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        
        std::unique_ptr<Device> device;
        std::unique_ptr<CommandQueue> commandQueue;
        std::unique_ptr<Swapchain> swapchain;
        std::unique_ptr<CommandAllocator> commandAllocator;
        std::unique_ptr<CommandList> commandList;
        std::unique_ptr<Fence> fence;
};