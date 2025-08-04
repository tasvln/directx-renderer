#pragma once

#include "utils/pch.h"
#include <queue>

// apparently every part of the commands have to be centralized and stored in the command queue 
// cuase there are multiple command allocators
// To::Do::need to find a work around this or a better and cleaner way of doing this

class CommandQueue {
    public:
        CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        ~CommandQueue() = default;

        // allocator
        ComPtr<ID3D12CommandAllocator> createCommandAllocator();

        // list
        ComPtr<ID3D12GraphicsCommandList2> createCommandList(
            ComPtr<ID3D12CommandAllocator> allocator
        );

        ComPtr<ID3D12GraphicsCommandList2> getCommandList();

        UINT64 executeCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList);

        // fence

        UINT64 signalFence();

        void fenceWait(UINT64 value);

        void fenceFlush(UINT64 value);

        bool isFenceComplete(UINT64 value);

        // getters
        ComPtr<ID3D12CommandQueue> getQueue() const {
            return queue;
        }

        ComPtr<ID3D12CommandAllocator> getAllocator() const {
            return allocator;
        }  

        ComPtr<ID3D12Fence> getFence() const {
            return fence;
        }

        UINT64 getFenceValue() const {
            return fenceValue;
        }

        HANDLE getFenceHandle() const {
            return fenceEvent;
        }
        
    private:
        ComPtr<ID3D12Device2> device;
        ComPtr<ID3D12CommandQueue> queue;
        ComPtr<ID3D12CommandAllocator> allocator;
        ComPtr<ID3D12GraphicsCommandList2> list;

        D3D12_COMMAND_LIST_TYPE type;

        // Keep track of command allocators that are "in-flight"
        struct AllocatorEntry
        {
            UINT64 fenceValue;
            ComPtr<ID3D12CommandAllocator> allocator;
        };
        
        std::queue<AllocatorEntry> allocatorQueue;
        std::queue<ComPtr<ID3D12GraphicsCommandList2>> listQueue;

        // fence
        ComPtr<ID3D12Fence> fence;
        UINT64 fenceValue = 0;
        HANDLE fenceEvent = nullptr;
};