#include "command_queue.h"

CommandQueue::CommandQueue(
    ComPtr<ID3D12Device2> device, 
    D3D12_COMMAND_LIST_TYPE type
) :
    fenceValue(0),
    type(type),
    device(device)  
{
    D3D12_COMMAND_QUEUE_DESC desc {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;
    
    throwFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
    LOG_INFO(L"CommandQueue -> Created Command Queue");
    throwFailed(
        device->CreateFence(
            fenceValue, 
            D3D12_FENCE_FLAG_NONE, 
            IID_PPV_ARGS(&fence)
        )
    );
    LOG_INFO(L"CommandQueue -> Created Fence");

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    LOG_INFO(L"CommandQueue -> Created Fence Event");
    LOG_INFO(L"CommandQueue Initialized");
}

CommandQueue::~CommandQueue() = default;

ComPtr<ID3D12CommandAllocator> CommandQueue::createCommandAllocator() {
    ComPtr<ID3D12CommandAllocator> allocator;
    throwFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
 
    return allocator;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::createCommandList(
    ComPtr<ID3D12CommandAllocator> allocator
) {
    ComPtr<ID3D12GraphicsCommandList2> commandList;

    throwFailed(
        device->CreateCommandList(
            0, 
            type, 
            allocator.Get(), 
            nullptr, 
            IID_PPV_ARGS(&commandList)
        )
    );

    throwFailed(commandList->Close());

    return commandList;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::getCommandList() {
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList2> commandList;

    if(!allocatorQueue.empty() && isFenceComplete(allocatorQueue.front().fenceValue)) {
        commandAllocator = allocatorQueue.front().allocator;
        allocatorQueue.pop();

        throwFailed(commandAllocator->Reset());
    } else {
        commandAllocator = createCommandAllocator();
    }

    if (!listQueue.empty()) {
        commandList = listQueue.front();
        listQueue.pop();

        throwFailed(commandList->Reset(commandAllocator.Get(), nullptr));
    } else {
        commandList = createCommandList(commandAllocator);
    }

    // Associate the command allocator with the command list so that it can be
    // retrieved when the command list is executed.
    throwFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

    return commandList;
}

UINT64 CommandQueue::executeCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList) {
    commandList->Close();

    ID3D12CommandAllocator* commandAllocator;
    UINT size = sizeof(commandAllocator);

    throwFailed(
        commandList->GetPrivateData(
            __uuidof(ID3D12CommandAllocator), 
            &size, 
            &commandAllocator
        )
    );

    ID3D12CommandList* const ppCommandLists[] = {
        commandList.Get()
    };

    queue->ExecuteCommandLists(
        1,
        ppCommandLists
    );

    UINT64 fenceValue = signalFence();

    allocatorQueue.emplace(
        AllocatorEntry{
            fenceValue,
            &commandAllocator
        }
    );

    listQueue.push(commandList);

    // The ownership of the command allocator has been transferred to the ComPtr
    // in the command allocator queue. It is safe to release the reference 
    // in this temporary COM pointer here.
    commandAllocator->Release();

    return fenceValue;
}

bool CommandQueue::isFenceComplete(UINT64 value) {
    return fence->GetCompletedValue() >= fenceValue;
}

void CommandQueue::fenceFlush(UINT64 value) {
    fenceWait(value);
}

UINT64 CommandQueue::signalFence() {
    LOG_INFO(L"fenceValue before++: %d", fenceValue);
    UINT64 fenceValue = fenceValue++;
    LOG_INFO(L"fenceValue after++: %d", fenceValue);
    throwFailed(queue->Signal(fence.Get(), fenceValue));
    LOG_INFO(L"fenceValue commandQueue->Signal: %d", fenceValue);
    return fenceValue;
}

void CommandQueue::fenceWait(UINT64 value) {
    if (!isFenceComplete(fenceValue))
    {
        throwFailed(fence->SetEventOnCompletion(value, fenceEvent));
        WaitForSingleObject(fenceEvent, DWORD_MAX);
    }
}
