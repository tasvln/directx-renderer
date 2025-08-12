#pragma once

#include "utils/pch.h"
#include "utils/frame_timer.h"
#include "core/window.h"

#include "device.h"
#include "command_queue.h"
#include "swapchain.h"

class Engine {
    public:
        static std::shared_ptr<Engine> getInstance(
            HINSTANCE hInstance = nullptr,
            WindowConfig config = {}
        ) {
            static std::weak_ptr<Engine> weakInstance;
            auto instance = weakInstance.lock();

            if (!instance) {
                // Only allow creation if we have valid parameters
                if (!hInstance) {
                    throw std::runtime_error("Engine must be created with valid HINSTANCE first.");
                }
                instance = std::shared_ptr<Engine>(new Engine(hInstance, config));
                weakInstance = instance;
            }
            return instance;
        }

        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        void init();

        void update();

        void render();

        void resize(uint32_t width, uint32_t height);

        void flush();

        UINT present();

        UINT getCurrentIndex();

        std::shared_ptr<CommandQueue> getCommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT) const;

        std::shared_ptr<Window> createRenderWindow(WindowConfig& config);

        D3D12_CPU_DESCRIPTOR_HANDLE getCurrentRTV() const {
            return CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentIndex, rtvDescriptorSize);
        }

    private:
        Engine(
            HINSTANCE hInstance,
            WindowConfig config
        );
        
        ~Engine();

        HINSTANCE hInstance;
        HWND hwnd = nullptr;
        WindowConfig config;

        UINT currentIndex{};
        UINT rtvDescriptorSize{};

        uint64_t frameFenceCounts[FRAMEBUFFERCOUNT]{};
        BOOL isTearingSupported = false;

        FrameTimer timer;

        // ComPtr<ID3D12Resource> backBuffers[FRAMEBUFFERCOUNT];
        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        
        std::unique_ptr<Device> device;

        std::shared_ptr<CommandQueue> directCommandQueue;
        std::shared_ptr<CommandQueue> computeCommandQueue;
        std::shared_ptr<CommandQueue> copyCommandQueue;

        std::unique_ptr<Swapchain> swapchain;
};