#include "application.h"
#include "window.h"

#include "engine/device.h"
#include "engine/command_queue.h"
#include "engine/swapchain.h"
#include "engine/mesh.h"

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
    LOG_INFO(L"Application -> Initializing...");

    scissorRect = CD3DX12_RECT(
        0, 
        0, 
        static_cast<LONG>(config.width), 
        static_cast<LONG>(config.height)
    );

    viewport = CD3DX12_VIEWPORT(
        0.0f, 
        0.0f, 
        static_cast<float>(config.width), 
        static_cast<float>(config.height)
    );

    device = std::make_unique<Device>(config.useWarp);
    LOG_INFO(L"Application -> device initialized!");

    directCommandQueue = std::make_unique<CommandQueue>(
        device->getDevice(),
        D3D12_COMMAND_LIST_TYPE_DIRECT
    );
    LOG_INFO(L"Application -> directCommandQueue initialized!");

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

    swapchain = std::make_unique<Swapchain>(
        window->getHwnd(),
        device->getDevice(),
        directCommandQueue->getQueue(),
        config.width,
        config.height,
        FRAMEBUFFERCOUNT,
        device->getSupportTearingState()
    );
    LOG_INFO(L"Application -> swapchain initialized!");

    currentBackBufferIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    LOG_INFO(L"Application Class initialized!");
    LOG_INFO(L"-- Resources --");

    std::vector<VertexStruct> vertices =
    {
        {{-1.0f, -1.0f, -1.0f}, {1,0,0}}, // red
        {{-1.0f, +1.0f, -1.0f}, {0,1,0}}, // green
        {{+1.0f, +1.0f, -1.0f}, {0,0,1}}, // blue
        {{+1.0f, -1.0f, -1.0f}, {1,1,0}}, // yellow
        {{-1.0f, -1.0f, +1.0f}, {1,0,1}}, // magenta
        {{-1.0f, +1.0f, +1.0f}, {0,1,1}}, // cyan
        {{+1.0f, +1.0f, +1.0f}, {1,1,1}}, // white
        {{+1.0f, -1.0f, +1.0f}, {0,0,0}}, // black
    };

    // 36 indices for cube (12 triangles)
    std::vector<uint32_t> indices =
    {
        0,1,2, 0,2,3,   // front
        4,6,5, 4,7,6,   // back
        4,5,1, 4,1,0,   // left
        3,2,6, 3,6,7,   // right
        1,5,6, 1,6,2,   // top
        4,0,3, 4,3,7    // bottom
    };

    // create buffers
    mesh = std::make_unique<Mesh>(
        device->getDevice(),
        vertices,
        indices
    );
    LOG_INFO(L"Mesh Resource initialized!");
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
    // swapchain->resize()
}

void Application::onUpdate()
{
    // Game/update logic
}

void Application::onRender()
{
    // Issue rendering commands
}