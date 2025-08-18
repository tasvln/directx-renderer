#include "engine.h"

// Engine::Engine(
//     HINSTANCE hInstance,
//     WindowConfig config
// ) :
//     // timer(),
//     hInstance(hInstance),
//     config(config)
// {
// }

// Engine::~Engine() {
//     flush();
// };

void Engine::init(
    HINSTANCE hInstance,
    WindowConfig &config,
    HWND hwnd)
{
    this->hInstance = hInstance;
    this->config = config;
    this->hwnd = hwnd;

    device = std::make_unique<Device>(
        config.useWarp);
    LOG_INFO(L"Engine->DirectX 12 device initialized.");

    directCommandQueue = std::make_shared<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_DIRECT);
    LOG_INFO(L"Engine->DirectX 12 directCommandQueue initialized.");

    computeCommandQueue = std::make_shared<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_COMPUTE);
    LOG_INFO(L"Engine->DirectX 12 computeCommandQueue initialized.");

    copyCommandQueue = std::make_shared<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_COPY);
    LOG_INFO(L"Engine->DirectX 12 copyCommandQueue initialized.");

    swapchain = std::make_unique<Swapchain>(
        hwnd,
        getCommandQueue()->getQueue(),
        config.width,
        config.height,
        FRAMEBUFFERCOUNT,
        device->getSupportTearingState());
    LOG_INFO(L"Engine->DirectX 12 swapchain initialized.");

    currentIndex = swapchain->getSwapchain().Get()->GetCurrentBackBufferIndex();
    LOG_INFO(L"Engine->DirectX 12 currentIndex set.");

    rtvHeap = swapchain->createDescriptorHeap(
        device->getDevice(),
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        FRAMEBUFFERCOUNT);
    LOG_INFO(L"Engine->DirectX 12 rtvHeap initialized.");

    rtvDescriptorSize = device->getDevice().Get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    LOG_INFO(L"Engine->DirectX 12 rtvDescriptorSize set.");

    swapchain->updateRTVs(
        device->getDevice(),
        swapchain->getSwapchain(),
        rtvHeap);
    LOG_INFO(L"Engine->DirectX 12 rtvs updated.");

    config.enabledDirectX = true;
}

UINT Engine::getCurrentIndex()
{
    return currentIndex;
}

UINT Engine::present()
{
    INT syncInterval = config.vsync ? 1 : 0;
    UINT presentFlags = isTearingSupported && !config.vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;

    LOG_INFO(L"Calling Present: vsync = %d, tearing = %d", config.vsync, isTearingSupported);
    throwFailed(
        swapchain->getSwapchain()->Present(
            syncInterval,
            presentFlags));
    currentIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    return currentIndex;
}

void Engine::flush()
{
    // fence->flush(commandQueue->getQueue());
    directCommandQueue->flush();
    computeCommandQueue->flush();
    copyCommandQueue->flush();
}

void Engine::resize(uint32_t width, uint32_t height)
{
    if (config.width != width || config.height != height)
    {
        config.width = std::max(1u, width);
        config.height = std::max(1u, height);

        // fence->flush(commandQueue->getQueue());
        flush();

        for (int i = 0; i < FRAMEBUFFERCOUNT; ++i)
        {
            swapchain->getBackBuffer(i).Reset();
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

        throwFailed(
            swapchain->getSwapchain().Get()->GetDesc(&swapChainDesc));

        throwFailed(
            swapchain->getSwapchain().Get()->ResizeBuffers(
                FRAMEBUFFERCOUNT,
                config.width,
                config.height,
                swapChainDesc.BufferDesc.Format,
                swapChainDesc.Flags));

        currentIndex = swapchain->getSwapchain().Get()->GetCurrentBackBufferIndex();

        swapchain->updateRTVs(
            device->getDevice(),
            swapchain->getSwapchain(),
            rtvHeap);
    }
}

std::shared_ptr<Window> Engine::createRenderWindow(
    WindowConfig &config)
{
    // You may want to cache windows in maps if you want multiple windows later -> window manager but not needed rn
    auto window = std::make_shared<Window>(hInstance, config);
    LOG_INFO(L"createRenderWindow TRIGGERED -> window created!");
    
    return window;
}

std::shared_ptr<CommandQueue> Engine::getCommandQueue(D3D12_COMMAND_LIST_TYPE type) const
{
    std::shared_ptr<CommandQueue> commandQueue;

    switch (type)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        commandQueue = directCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        commandQueue = computeCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        commandQueue = copyCommandQueue;
        break;
    default:
        LOG_ERROR(L"Invalid command queue type");
    }

    return commandQueue;
}