#include "command_allocator.h"

CommandAllocator::CommandAllocator(
    ComPtr<ID3D12Device2> device,
    D3D12_COMMAND_LIST_TYPE type
) {
    allocator = createCommandAllocator(device, type);
    LOG_INFO(L"CommandAllocator->Created Command Allocator");
}

ComPtr<ID3D12CommandAllocator> CommandAllocator::createCommandAllocator(
    ComPtr<ID3D12Device2> device,
    D3D12_COMMAND_LIST_TYPE type
) {
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    throwFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));
 
    return commandAllocator;
}