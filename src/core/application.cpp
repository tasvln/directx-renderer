#include "application.h"
#include "window.h"

#include "engine/device.h"
#include "engine/command_queue.h"
#include "engine/swapchain.h"
#include "engine/mesh.h"
#include "engine/buffer/constant.h"
#include "engine/shader.h"
#include "engine/pipeline.h"
#include "engine/scene/camera.h"

#include "utils/events.h"
#include "utils/frame_timer.h"

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
        directCommandQueue->getCommandQueue(),
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

    // mvpBuffer?
    constantBuffer1 = std::make_unique<ConstantBuffer>(
        device->getDevice(),
        static_cast<UINT>(sizeof(ConstantMVP))
    );
    LOG_INFO(L"ConstantBuffer1 Resource initialized!");

    camera1 = std::make_unique<Camera>(
        45.0f,
        static_cast<float>(config.width) / static_cast<float>(config.height),
        0.1f,
        100.0f
    );
    LOG_INFO(L"Camera initialized!");

    XMMATRIX view = camera1->getViewMatrix();
    XMMATRIX proj = camera1->getProjectionMatrix();
    LOG_INFO(L"Camera View matrix[0][0]: %f", view.r[0].m128_f32[0]);
    LOG_INFO(L"Camera Projection matrix[0][0]: %f", proj.r[0].m128_f32[0]);

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
    Timer timer;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            timer.tick();

            UpdateEventArgs updateArgs(
                timer.getDeltaSeconds(), 
                timer.getTotalSeconds()
            );
            onUpdate(updateArgs);

            RenderEventArgs renderArgs(
                timer.getDeltaSeconds(), 
                timer.getTotalSeconds()
            );
            onRender(renderArgs);
            
            std::wstring title = L"DirectX12 App - " + timer.getFPSString();
            if (window) {
                SetWindowTextW(window->getHwnd(), title.c_str());
            } else {
                LOG_ERROR(L"Window is null!");
            }
        }
    }

    return static_cast<int>(msg.wParam);
}

void Application::onUpdate(UpdateEventArgs& args)
{
    // LOG_INFO(L"[onUpdate] Δt=%.4f sec | total=%.2f sec", args.elapsedTime, args.totalTime);

    camera1->update(static_cast<float>(args.totalTime));

    // Build MVP
    XMMATRIX model = XMMatrixIdentity();
    XMMATRIX view = camera1->getViewMatrix();
    XMMATRIX projection = camera1->getProjectionMatrix();

    ConstantMVP mvpData;
    mvpData.mvp = XMMatrixTranspose(model * view * projection);

    // Upload to GPU
    constantBuffer1->update(&mvpData, sizeof(mvpData));
}

void Application::onRender(RenderEventArgs& args)
{
    // LOG_INFO(L"[onRender] Δt=%.4f sec | total=%.2f sec", args.elapsedTime, args.totalTime);

    auto commandAllocator = directCommandQueue->getCommandAllocator();
    auto commandList = directCommandQueue->getCommandList();
    auto commandQueue = directCommandQueue->getCommandQueue();

    auto pipelineState = pipeline1->getPipelineState();
    auto rootSignature = pipeline1->getRootSignature();

    auto rtvHeap = swapchain->getRTVHeap();
    auto dsvHeap = swapchain->getDSVHeap();

    auto vertex = mesh->getVertex();
    auto index = mesh->getIndex();

    auto vsync = device->getSupportTearingState();

    // Reset the queue -> allocator and list
    throwFailed(
        commandAllocator->Reset()
    );
    throwFailed(
        commandList->Reset(
            commandAllocator.Get(), 
            pipelineState.Get()
        )
    );

    // Set root signature
    commandList->SetGraphicsRootSignature(
        rootSignature.Get()
    );

    // Update constant buffer (MVP)
    XMMATRIX model = XMMatrixIdentity();
    XMMATRIX view = camera1->getViewMatrix();
    XMMATRIX projection = camera1->getProjectionMatrix();

    ConstantMVP mvpData;
    mvpData.mvp = XMMatrixTranspose(model * view * projection);
    constantBuffer1->update(&mvpData, sizeof(mvpData));

    commandList->SetGraphicsRootConstantBufferView(
        0, 
        constantBuffer1->getGPUAddress()
    );

    // Set viewport and scissor
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    // Indicate render target state
    auto backBuffer = swapchain->getBackBuffer(currentBackBufferIndex);
    transitionResource(
        commandList,
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );

    // Set RTV and DSV
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        rtvHeap->getHeap()->GetCPUDescriptorHandleForHeapStart(),
        currentBackBufferIndex, 
        rtvHeap->getDescriptorSize()
    );

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
        dsvHeap->getHeap()->GetCPUDescriptorHandleForHeapStart()
    );

    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // Clear render target and depth buffer
    const float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

    /* Drawing the Triangles -> or where the main drawing happens especially with rendering models */
    // Set primitive topology
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Bind vertex/index buffers
    commandList->IASetVertexBuffers(0, 1, &vertex->getView());
    commandList->IASetIndexBuffer(&index->getView());

    // Draw
    commandList->DrawIndexedInstanced(index->getCount(), 1, 0, 0, 0);

    // Present
    transitionResource(
        commandList,
        backBuffer.Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );

    fenceValues[currentBackBufferIndex] = directCommandQueue->executeCommandList(commandList);

    INT syncInterval = vsync ? 1 : 0;
    UINT presentFlags = !vsync ? DXGI_PRESENT_ALLOW_TEARING : 0;

    throwFailed(
        swapchain->getSwapchain()->Present(
            syncInterval,
            presentFlags
        )
    );

    currentBackBufferIndex = swapchain->getSwapchain()->GetCurrentBackBufferIndex();

    // the CPU immediately waits for the GPU to finish this frame before moving on
    directCommandQueue->fenceWait(fenceValues[currentBackBufferIndex]);
}

void Application::transitionResource(
    ComPtr<ID3D12GraphicsCommandList2> commandList,
    ComPtr<ID3D12Resource> resource,
    D3D12_RESOURCE_STATES beforeState,
    D3D12_RESOURCE_STATES afterState
) {
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        resource.Get(),
        beforeState,
        afterState
    );

    commandList->ResourceBarrier(1, &barrier);
}

void Application::onResize(UINT width, UINT height)
{
    // LOG_INFO(L"Application resize: %dx%d", width, height);
    // Resize swapchain & depth buffer here
    // swapchain->resize()
}