#include "swapchain.h"

Swapchain::Swapchain(
    HWND hwnd, 
    ComPtr<ID3D12CommandQueue> commandQueue, 
    uint32_t width, 
    uint32_t height, 
    uint32_t bufferCount, 
    bool tearingSupport
) {
    swapchain = createSwapchain(
        hwnd,
        commandQueue,
        width,
        height,
        bufferCount,
        tearingSupport
    );
    
    LOG_INFO(L"Swapchain->Created Swapchain");
}

ComPtr<IDXGISwapChain4>Swapchain::createSwapchain(
    HWND& hwnd, 
    ComPtr<ID3D12CommandQueue> commandQueue, 
    uint32_t width, 
    uint32_t height, 
    uint32_t bufferCount, 
    bool tearingSupport
) {
    ComPtr<IDXGISwapChain4> dxgiSwapChain4;
    ComPtr<IDXGIFactory4> dxgiFactory4;
    UINT createFactoryFlags = 0;

    #if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif

    throwFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.Stereo = FALSE;
    swapChainDesc.SampleDesc = { 
        1, 0 
    };
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = bufferCount;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    // It is recommended to always allow tearing if tearing support is available.
    swapChainDesc.Flags = tearingSupport ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain1;

    throwFailed(
        dxgiFactory4->CreateSwapChainForHwnd(
            commandQueue.Get(),
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        )
    );
 
    // Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
    // will be handled manually.
    throwFailed(dxgiFactory4->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
 
    throwFailed(swapChain1.As(&dxgiSwapChain4));
 
    return dxgiSwapChain4;
}

ComPtr<ID3D12DescriptorHeap> Swapchain::createDescriptorHeap(
    ComPtr<ID3D12Device2> device,
    D3D12_DESCRIPTOR_HEAP_TYPE type, 
    uint32_t numDescriptors
) {
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;
 
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = numDescriptors;
    desc.Type = type;
 
    throwFailed(
        device->CreateDescriptorHeap(
            &desc, 
            IID_PPV_ARGS(&descriptorHeap)
        )
    );
 
    return descriptorHeap;
}


// For each back buffer of the swap chain, a single RTV is used to describe the resource.
void Swapchain::updateRTVs(
    ComPtr<ID3D12Device2> device,
    ComPtr<IDXGISwapChain4> swapChain, 
    ComPtr<ID3D12DescriptorHeap> descriptorHeap
) {
    auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
 
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());
 
    for (int i = 0; i < FRAMEBUFFERCOUNT; ++i)
    {
        ComPtr<ID3D12Resource> backBuffer;
        throwFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));
 
        device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
 
        backBuffers[i] = backBuffer;
 
        rtvHandle.Offset(rtvDescriptorSize);
    }
}