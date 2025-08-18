#include "command_queue.h"

CommandQueue::CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
    : device(device), type(type), fenceValue(0)
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    throwFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
    throwFailed(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    LOG_INFO(L"CommandQueue Initialized");
}

ComPtr<ID3D12CommandAllocator> CommandQueue::createCommandAllocator() {
    ComPtr<ID3D12CommandAllocator> allocator;
    throwFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
    return allocator;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::createCommandList(ComPtr<ID3D12CommandAllocator> allocator) {
    ComPtr<ID3D12GraphicsCommandList2> list;
    throwFailed(device->CreateCommandList(0, type, allocator.Get(), nullptr, IID_PPV_ARGS(&list)));
    throwFailed(list->Close());
    return list;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::getCommandList() {
    ComPtr<ID3D12CommandAllocator> alloc;
    ComPtr<ID3D12GraphicsCommandList2> cmdList;

    if (!allocatorQueue.empty() && isFenceComplete(allocatorQueue.front().fenceValue)) {
        alloc = allocatorQueue.front().allocator;
        allocatorQueue.pop();
        throwFailed(alloc->Reset());
    } else {
        alloc = createCommandAllocator();
    }

    if (!listQueue.empty()) {
        cmdList = listQueue.front();
        listQueue.pop();
        throwFailed(cmdList->Reset(alloc.Get(), nullptr));
    } else {
        cmdList = createCommandList(alloc);
    }

    throwFailed(cmdList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), alloc.Get()));
    return cmdList;
}

UINT64 CommandQueue::executeCommandList(ComPtr<ID3D12GraphicsCommandList2> cmdList) {
    throwFailed(cmdList->Close());

    ID3D12CommandAllocator* allocPtr;
    UINT size = sizeof(allocPtr);
    throwFailed(cmdList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &size, &allocPtr));

    ID3D12CommandList* lists[] = { cmdList.Get() };
    queue->ExecuteCommandLists(1, lists);

    UINT64 val = signalFence();
    allocatorQueue.emplace(AllocatorEntry{ val, allocPtr });
    listQueue.push(cmdList);

    allocPtr->Release();
    return val;
}

bool CommandQueue::isFenceComplete(UINT64 value) {
    return fence->GetCompletedValue() >= value;
}

void CommandQueue::fenceWait(UINT64 value) {
    if (!isFenceComplete(value)) {
        throwFailed(fence->SetEventOnCompletion(value, fenceEvent));
        WaitForSingleObject(fenceEvent, DWORD_MAX);
    }
}

void CommandQueue::fenceFlush(UINT64 value) {
    fenceWait(value);
}

UINT64 CommandQueue::signalFence() {
    fenceValue++;
    LOG_INFO(L"Signaling fenceValue: %llu", fenceValue);
    throwFailed(queue->Signal(fence.Get(), fenceValue));
    return fenceValue;
}

void CommandQueue::flush() {
    fenceWait(signalFence());
}
