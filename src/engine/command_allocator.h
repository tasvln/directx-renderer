#pragma once

#include "utils/pch.h"

class CommandAllocator {
    public:
        CommandAllocator(
            ComPtr<ID3D12Device2> device,
            D3D12_COMMAND_LIST_TYPE type
        );
        ~CommandAllocator() = default;

        ComPtr<ID3D12CommandAllocator> createCommandAllocator(
            ComPtr<ID3D12Device2> device,
            D3D12_COMMAND_LIST_TYPE type
        );

        ComPtr<ID3D12CommandAllocator> getAllocator() const {
            return allocator;
        }   

    private:
        ComPtr<ID3D12CommandAllocator> allocator;
};