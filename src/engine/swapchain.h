#pragma once

#include "utils/pch.h"

class Swapchain {
    public:
        Swapchain(
            HWND hwnd, 
            ComPtr<ID3D12CommandQueue> commandQueue, 
            uint32_t width, 
            uint32_t height, 
            uint32_t bufferCount, 
            bool tearingSupport
        );

        ~Swapchain() = default;

        ComPtr<IDXGISwapChain4> createSwapchain(
            HWND& hwnd, 
            ComPtr<ID3D12CommandQueue> commandQueue, 
            uint32_t width, 
            uint32_t height, 
            uint32_t bufferCount, 
            bool tearingSupport
        );

        ComPtr<ID3D12DescriptorHeap> createDescriptorHeap(
            ComPtr<ID3D12Device2> device,
            D3D12_DESCRIPTOR_HEAP_TYPE type, 
            uint32_t numDescriptors
        );

        void updateRTVs(
            ComPtr<ID3D12Device2> device,
            ComPtr<IDXGISwapChain4> swapChain, 
            ComPtr<ID3D12DescriptorHeap> descriptorHeap
        );

        // bool checkTearingSupport();

        ComPtr<IDXGISwapChain4> getSwapchain() const {
            return swapchain;
        }
    private:
        ComPtr<IDXGISwapChain4> swapchain;
        ComPtr<ID3D12Resource> backBuffers[FRAMEBUFFERCOUNT];
};