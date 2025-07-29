#pragma once

#include "utils/pch.h"

class CommandList {
    public:
        CommandList(
            ComPtr<ID3D12Device2> device,
            ComPtr<ID3D12CommandAllocator> commandAllocator, 
            D3D12_COMMAND_LIST_TYPE type
        );
        ~CommandList() = default;

        ComPtr<ID3D12GraphicsCommandList> createCommandList(
            ComPtr<ID3D12Device2> device,
            ComPtr<ID3D12CommandAllocator> commandAllocator, 
            D3D12_COMMAND_LIST_TYPE type
        );

        ComPtr<ID3D12GraphicsCommandList> getCommandList() {
            return list;
        }

    private:
        ComPtr<ID3D12GraphicsCommandList> list;
};