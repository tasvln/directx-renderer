#include "command_list.h"

CommandList::CommandList(
    ComPtr<ID3D12Device2> device,
    ComPtr<ID3D12CommandAllocator> commandAllocator, 
    D3D12_COMMAND_LIST_TYPE type
) {
    list = createCommandList(
        device,
        commandAllocator,
        type
    );

    LOG_INFO(L"CommandList->Created Command List");
}

ComPtr<ID3D12GraphicsCommandList> CommandList::createCommandList(
    ComPtr<ID3D12Device2> device,
    ComPtr<ID3D12CommandAllocator> commandAllocator, 
    D3D12_COMMAND_LIST_TYPE type
) {
    ComPtr<ID3D12GraphicsCommandList> commandList;

    throwFailed(
        device->CreateCommandList(
            0, 
            type, 
            commandAllocator.Get(), 
            nullptr, 
            IID_PPV_ARGS(&commandList)
        )
    );

    throwFailed(commandList->Close());

    return commandList;
}
