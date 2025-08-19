#include "command_queue.h"

CommandQueue::CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
    : device(device), type(type), fenceValue(0)
{
    LOG_INFO(L"Initializing CommandQueue of type %d", type);

    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    throwFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue)));
    LOG_INFO(L"CommandQueue created");

    throwFailed(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    LOG_INFO(L"Fence created with initial value %llu", fenceValue);

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    LOG_INFO(L"CommandQueue Initialized successfully");
}

ComPtr<ID3D12CommandAllocator> CommandQueue::createCommandAllocator() {
    ComPtr<ID3D12CommandAllocator> allocator;
    throwFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&allocator)));
    LOG_INFO(L"Created new CommandAllocator");
    return allocator;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::createCommandList(ComPtr<ID3D12CommandAllocator> allocator) {
    ComPtr<ID3D12GraphicsCommandList2> list;
    throwFailed(device->CreateCommandList(0, type, allocator.Get(), nullptr, IID_PPV_ARGS(&list)));
    throwFailed(list->Close());
    LOG_INFO(L"Created new CommandList and closed it initially");
    return list;
}

ComPtr<ID3D12GraphicsCommandList2> CommandQueue::getCommandList() {
    LOG_INFO(L"Getting a CommandList from pool");
    ComPtr<ID3D12CommandAllocator> alloc;
    ComPtr<ID3D12GraphicsCommandList2> cmdList;

    if (!allocatorQueue.empty() && isFenceComplete(allocatorQueue.front().fenceValue)) {
        alloc = allocatorQueue.front().allocator;
        allocatorQueue.pop();
        throwFailed(alloc->Reset());
        LOG_INFO(L"Reusing CommandAllocator from pool");
    } else {
        alloc = createCommandAllocator();
    }

    if (!listQueue.empty()) {
        cmdList = listQueue.front();
        listQueue.pop();
        throwFailed(cmdList->Reset(alloc.Get(), nullptr));
        LOG_INFO(L"Reusing CommandList from pool");
    } else {
        cmdList = createCommandList(alloc);
    }

    throwFailed(cmdList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), alloc.Get()));
    return cmdList;
}

UINT64 CommandQueue::executeCommandList(ComPtr<ID3D12GraphicsCommandList2> cmdList) {
    LOG_INFO(L"Executing CommandList");
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
    LOG_INFO(L"CommandList executed, fence signaled with value %llu", val);
    return val;
}

bool CommandQueue::isFenceComplete(UINT64 value) {
    bool complete = fence->GetCompletedValue() >= value;
    LOG_INFO(L"Fence check: %llu complete? %d", value, complete);
    return complete;
}

void CommandQueue::fenceWait(UINT64 value) {
    LOG_INFO(L"Waiting for fence %llu", value);
    if (!isFenceComplete(value)) {
        throwFailed(fence->SetEventOnCompletion(value, fenceEvent));
        WaitForSingleObject(fenceEvent, DWORD_MAX);
        LOG_INFO(L"Fence %llu completed", value);
    }
}

void CommandQueue::fenceFlush(UINT64 value) {
    LOG_INFO(L"Flushing fence %llu", value);
    fenceWait(value);
}

UINT64 CommandQueue::signalFence() {
    fenceValue++;
    LOG_INFO(L"Signaling fenceValue: %llu", fenceValue);
    throwFailed(queue->Signal(fence.Get(), fenceValue));
    return fenceValue;
}

void CommandQueue::flush() {
    LOG_INFO(L"Flushing CommandQueue");
    fenceWait(signalFence());
}
