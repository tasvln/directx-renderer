#include "command_queue.h"

CommandQueue::CommandQueue(
    ComPtr<ID3D12Device2> device, 
    D3D12_COMMAND_LIST_TYPE type
) {
    queue = createQueue(device, type);
    LOG_INFO(L"DirectX 12 Command Queue initialized.");
}

CommandQueue::~CommandQueue() = default;

ComPtr<ID3D12CommandQueue> CommandQueue::createQueue(
    ComPtr<ID3D12Device2> device, 
    D3D12_COMMAND_LIST_TYPE type
) {
    ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

    D3D12_COMMAND_QUEUE_DESC desc {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;
    
    throwFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

    return d3d12CommandQueue;
}