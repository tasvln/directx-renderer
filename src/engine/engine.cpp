#include "engine.h"

Engine::Engine(
    HWND& hwnd,
    WindowConfig& config,
    RECT& windowRect
) : 
    timer(),
    config(config),
    hwnd(hwnd),
    windowRect(windowRect)
{
    device = std::make_unique<Device>(
        config.useWarp
    );
    LOG_INFO(L"Engine->DirectX 12 device initialized.");

    commandQueue = std::make_unique<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_DIRECT
    );
    LOG_INFO(L"Engine->DirectX 12 commandQueue initialized.");

    swapchain = std::make_unique<Swapchain>(
        hwnd,
        commandQueue->getQueue(),
        config.width,
        config.height,
        FRAMEBUFFERCOUNT,
        device->getSupportTearingState()
    );
    LOG_INFO(L"Engine->DirectX 12 swapchain initialized.");

    currentIndex = swapchain->getSwapchain().Get()->GetCurrentBackBufferIndex();
    LOG_INFO(L"Engine->DirectX 12 currentIndex set.");

    rtvHeap = swapchain->createDescriptorHeap(
        device->getDevice(),
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        FRAMEBUFFERCOUNT
    );
    LOG_INFO(L"Engine->DirectX 12 rtvHeap initialized.");

    rtvDescriptorSize = device->getDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    LOG_INFO(L"Engine->DirectX 12 rtvDescriptorSize set.");

    swapchain->updateRTVs(
        device->getDevice(),
        swapchain->getSwapchain(),
        rtvHeap
    );
    LOG_INFO(L"Engine->DirectX 12 rtvs updated.");

    commandAllocators.resize(FRAMEBUFFERCOUNT);
    LOG_INFO(L"Engine->DirectX 12 commandAllocators resized");

    for (int i = 0; i < FRAMEBUFFERCOUNT; ++i) {
        commandAllocators[i] = std::make_unique<CommandAllocator>(
            device->getDevice(), 
            D3D12_COMMAND_LIST_TYPE_DIRECT
        );
    }
    LOG_INFO(L"Engine->DirectX 12 commandAllocators set.");

    commandList = std::make_unique<CommandList>(
        device->getDevice(), 
        commandAllocators[currentIndex]->getAllocator(), 
        D3D12_COMMAND_LIST_TYPE_DIRECT
    );
    LOG_INFO(L"Engine->DirectX 12 commandList initialized.");

    fence = std::make_unique<Fence>(device->getDevice());
    LOG_INFO(L"Engine->DirectX 12 fence initialized.");

    config.enabledDirectX = true;
}

Engine::~Engine() = default;

void Engine::init() {
    // MessageBox(0, L"Engine initialized", 0, 0);
    LOG_INFO(L"Engine::init()");
}

void Engine::update() {
    timer.tick();
    OutputDebugString(timer.getFPSString().c_str());
}

void Engine::render() {
    auto allocator = commandAllocators[currentIndex]->getAllocator();
    auto backBuffer = swapchain->getBackBuffer(currentIndex);

    if (!backBuffer) {
        LOG_ERROR(L"backBuffer at index %d is null!", currentIndex);
        return;
    }

    allocator->Reset();
    commandList->getCommandList()->Reset(allocator.Get(), nullptr);

    // clear
    D3D12_RESOURCE_BARRIER clearBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    commandList->getCommandList().Get()->ResourceBarrier(1, &clearBarrier);

    FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
        rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        currentIndex, 
        rtvDescriptorSize
    );

    commandList->getCommandList().Get()->ClearRenderTargetView(
        rtv,
        clearColor,
        0,
        nullptr
    );

    // present
    D3D12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    commandList->getCommandList().Get()->ResourceBarrier(1, &presentBarrier);

    throwFailed(commandList->getCommandList().Get()->Close());

    ID3D12CommandList* const commandLists[] = {
        commandList->getCommandList().Get()
    };

    commandQueue->getQueue().Get()->ExecuteCommandLists(
        _countof(commandLists), 
        commandLists
    );

    frameFenceCounts[currentIndex] = fence->signal(
        commandQueue->getQueue()
    );

    INT syncInterval = config.vsync ? 1 : 0;
    UINT presentFlags = isTearingSupported && !config.vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;

    LOG_INFO(L"Calling Present: vsync = %d, tearing = %d", config.vsync, isTearingSupported);
    throwFailed(
        swapchain->getSwapchain()->Present(
            syncInterval,
            presentFlags
        )
    );
    currentIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    fence->wait(frameFenceCounts[currentIndex]);
}

void Engine::flush() {
    fence->flush(commandQueue->getQueue());
}

void Engine::resize(uint32_t width, uint32_t height) {
    if (config.width != width || config.height != height) {
        config.width = std::max(1u, width);
        config.height = std::max(1u, height);

        fence->flush(commandQueue->getQueue());

        for (int i = 0; i < FRAMEBUFFERCOUNT; ++i)
        {
             swapchain->getBackBuffer(i).Reset();
            frameFenceCounts[i] = frameFenceCounts[currentIndex];
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

        throwFailed(
            swapchain->getSwapchain().Get()->GetDesc(&swapChainDesc)
        );

        throwFailed(
            swapchain->getSwapchain().Get()->ResizeBuffers(
                FRAMEBUFFERCOUNT,
                config.width,
                config.height,
                swapChainDesc.BufferDesc.Format,
                swapChainDesc.Flags
            )
        );

        currentIndex = swapchain->getSwapchain().Get()->GetCurrentBackBufferIndex();

        swapchain->updateRTVs(
            device->getDevice(),
            swapchain->getSwapchain(),
            rtvHeap
        );
    }
}

void Engine::setFullScreen(bool toggleFullscreen) {
    if (config.fullscreen != toggleFullscreen) {
        config.fullscreen = toggleFullscreen;

        if (config.fullscreen) {
            // Store the current window dimensions so they can be restored 
            // when switching out of fullscreen state.
            GetWindowRect(hwnd, &windowRect);

            // Set the window style to a borderless window so the client area fills
            // the entire screen.
            UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
 
            SetWindowLongW(hwnd, GWL_STYLE, windowStyle);

            // Query the name of the nearest display device for the window.
            // This is required to set the fullscreen dimensions of the window
            // when using a multi-monitor setup.
            HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFOEX monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFOEX);
            GetMonitorInfo(hMonitor, &monitorInfo);

            SetWindowPos(
                hwnd, 
                HWND_TOP,
                monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.top,
                monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
                monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
                SWP_FRAMECHANGED | SWP_NOACTIVATE
            );

            ShowWindow(hwnd, SW_MAXIMIZE);
        } 
    }
}
