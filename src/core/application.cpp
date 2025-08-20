#include "application.h"
#include "window.h"

#include "engine/device.h"
#include "engine/command_queue.h"
#include "engine/swapchain.h"
#include "engine/mesh.h"
#include "engine/buffer/constant.h"
#include "engine/shader.h"
#include "engine/pipeline.h"

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

    std::vector<VertexStruct> vertices = {
        {XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)}, // 0
        {XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)},  // 1
        {XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f)},   // 2
        {XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)},  // 3
        {XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)},  // 4
        {XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f)},   // 5
        {XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)},    // 6
        {XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f)}    // 7
    };

    // 36 indices for cube (12 triangles)
    std::vector<uint32_t> indices =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };

    // create buffers
    mesh = std::make_unique<Mesh>(
        device->getDevice(),
        vertices,
        indices
    );
    LOG_INFO(L"Mesh Resource initialized!");

    UINT constantBuffer1Size = 256;

    constantBuffer1 = std::make_unique<ConstantBuffer>(
        device->getDevice(),
        constantBuffer1Size
    );
    LOG_INFO(L"ConstantBuffer1 Resource initialized!");

    // pipeline
    // Root parameter for CBV
    CD3DX12_ROOT_PARAMETER cbvRootParam;
    cbvRootParam.InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    std::vector<D3D12_ROOT_PARAMETER> rootParams = { cbvRootParam };

    auto vertexShader = Shader(L"assets/shaders/vertex.hlsl", "vsmain", "vs_5_0");
    auto pixelShader = Shader(L"assets/shaders/pixel.hlsl", "psmain", "ps_5_0");

    auto checkShaderBytecode = [](const Shader& shader, const std::wstring& name) {
        auto bytecode = shader.getBytecode();
        if (!bytecode || bytecode->GetBufferSize() == 0) {
            LOG_ERROR(L"%s shader bytecode is null or empty!", name.c_str());
            throw std::runtime_error("Shader compilation failed");
        } else {
            LOG_INFO(L"%s shader compiled successfully. Bytecode size: %llu bytes", name.c_str(), bytecode->GetBufferSize());
        }

        auto errorBlob = shader.getErrorBlob();
        if (errorBlob && errorBlob->GetBufferSize() > 0) {
            std::string errMsg((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            OutputDebugStringA(errMsg.c_str());
            LOG_WARNING(L"%s shader compilation warnings/errors:\n%S", name.c_str(), errMsg.c_str());
        }
    };

    // Check vertex and pixel shaders
    checkShaderBytecode(vertexShader, L"Vertex");
    checkShaderBytecode(pixelShader, L"Pixel");

    pipeline1 = std::make_unique<Pipeline>(
        device->getDevice(),
        vertexShader,
        pixelShader,
        inputLayout,
        rootParams
    );
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