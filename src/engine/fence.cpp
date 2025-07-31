#include "fence.h"

Fence::Fence(ComPtr<ID3D12Device2> device) {
    fence = createFence(device);
    fenceEvent = createEvent();
    LOG_INFO(L"Fence->Fence & Fence Event created.");
}

Fence::~Fence() {
    CloseHandle(fenceEvent);
}

ComPtr<ID3D12Fence> Fence::createFence(ComPtr<ID3D12Device2> device) {
    ComPtr<ID3D12Fence> fence;
 
    throwFailed(
        device->CreateFence(
            0, 
            D3D12_FENCE_FLAG_NONE, 
            IID_PPV_ARGS(&fence)
        )
    );
 
    return fence;
}

HANDLE Fence::createEvent() {
    HANDLE fenceEvent;
    
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
   
    return fenceEvent;
}

void Fence::flush(ComPtr<ID3D12CommandQueue> commandQueue) {
    UINT64 val = signal(commandQueue);
    wait(val);
}

UINT64 Fence::signal(
    ComPtr<ID3D12CommandQueue> commandQueue
) {
    LOG_INFO(L"fenceCount before++: %d", fenceCount);
    fenceCount++;
    LOG_INFO(L"fenceCount after++: %d", fenceCount);
    throwFailed(commandQueue->Signal(fence.Get(), fenceCount));
    LOG_INFO(L"fenceCount commandQueue->Signal: %d", fenceCount);
    return fenceCount;
}

void Fence::wait(
    UINT64 value,
    std::chrono::milliseconds duration
) {
    if (fence->GetCompletedValue() < value)
    {
        throwFailed(fence->SetEventOnCompletion(value, fenceEvent));
        WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
    }
}