#pragma once

#include "utils/pch.h"

class CommandQueue {
    public:
        CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        ~CommandQueue();

        ComPtr<ID3D12CommandQueue> createQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);

        ComPtr<ID3D12CommandQueue> getQueue() const {
            return queue;
        }
        
    private:
        ComPtr<ID3D12CommandQueue> queue;
};