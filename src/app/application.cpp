#include "application.h"
#include "window.h"
#include "engine/device.h"
#include "engine/command_queue.h"

Application::Application(
    HINSTANCE hInstance, 
    WindowConfig &config
) :  
    config(config) 
{
    window = std::make_unique<Window>(hInstance, config, this);

    init();
}

Application::~Application() {

}

void Application::init() {
    LOG_INFO(L"Initializing Engine Classes");

    device = std::make_unique<Device>(config.useWarp);
    LOG_INFO(L"Engine->DirectX 12 device initialized.");

    directCommandQueue = std::make_unique<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_DIRECT
    );
    LOG_INFO(L"Engine->DirectX 12 directCommandQueue initialized.");

    // computeCommandQueue = std::make_unique<CommandQueue>(
    //     device->getDevice(),
    //     D3D12_COMMAND_LIST_TYPE_COMPUTE
    // );
    // LOG_INFO(L"Engine->DirectX 12 computeCommandQueue initialized.");

    // copyCommandQueue = std::make_unique<CommandQueue>(
    //     device->getDevice(),
    //     D3D12_COMMAND_LIST_TYPE_COPY
    // );
    // LOG_INFO(L"Engine->DirectX 12 copyCommandQueue initialized.");
}

int Application::run() {
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // else
        // {
        //     onUpdate();
        //     onRender();
        // }
    }

    return static_cast<int>(msg.wParam);
}

void Application::onResize(UINT width, UINT height)
{
    // LOG_INFO(L"Application resize: %dx%d", width, height);
    // Resize swapchain & depth buffer here
}

void Application::onUpdate()
{
    // Game/update logic
}

void Application::onRender()
{
    // Issue rendering commands
}