#pragma once

#include "utils/pch.h"

class Fence {
    public:
        Fence(ComPtr<ID3D12Device2> device);
        ~Fence();

        ComPtr<ID3D12Fence> createFence(ComPtr<ID3D12Device2> device);

        HANDLE createEvent();

        UINT64 signal(
            ComPtr<ID3D12CommandQueue> commandQueue
        );

        void wait(UINT64 value, std::chrono::milliseconds duration = std::chrono::milliseconds::max());

        void flush(ComPtr<ID3D12CommandQueue> commandQueue);

        ComPtr<ID3D12Fence> getFence() const {
            return fence;
        }

        UINT64 getFenceCount() const {
            return fenceCount;
        }

    private:
        ComPtr<ID3D12Fence> fence;
        UINT64 fenceCount = 0;
        HANDLE fenceEvent = nullptr;
};