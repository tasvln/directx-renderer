#include "device.h"

void Device::initialize(bool useWarp) {
    enableDebugLayer();

    adapter = selectAdapter(useWarp);
    device = createDevice(adapter);

    LOG_INFO(L"DirectX 12 device initialized.");
}

ComPtr<IDXGIAdapter4> Device::selectAdapter(bool useWarp) {
    ComPtr<IDXGIFactory4> dxgiFactory;
    ComPtr<IDXGIAdapter1> dxgiAdapter1;
    ComPtr<IDXGIAdapter4> dxgiAdapter4;

    UINT createFactoryFlags = 0;

    #if defined(_DEBUG)
        createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
    #endif

    throwFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

    if (useWarp)
    {
        throwFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
        throwFailed(dxgiAdapter1.As(&dxgiAdapter4));
    } else {
        size_t maxDedicatedVideoMemory = 0;

        // enumerate the available GPU adapters in the system
        for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            DXGI_ADAPTER_DESC1 desc;
            dxgiAdapter1->GetDesc1(&desc);

            // Check to see if the adapter can create a D3D12 device without actually 
            // creating it. The adapter with the largest dedicated video memory
            // is favored.
            
            if ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
                SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
                desc.DedicatedVideoMemory > maxDedicatedVideoMemory
            ) {
                maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
                throwFailed(dxgiAdapter1.As(&dxgiAdapter4));
            }
        }
    }

    if (!dxgiAdapter4) {
        LOG_ERROR(L"No compatible GPU adapter found.");
        throw std::runtime_error("No compatible GPU adapter found.");
    }

    return dxgiAdapter4;
}

ComPtr<ID3D12Device2> Device::createDevice(ComPtr<IDXGIAdapter4> deviceAdapter) {
    ComPtr<ID3D12Device2> id3d12device;

    // Try feature levels from highest to lowest
    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    for (auto level : featureLevels) {
        if (SUCCEEDED(D3D12CreateDevice(deviceAdapter.Get(), level, IID_PPV_ARGS(&id3d12device)))) {
            LOG_INFO(L"Created D3D12 device with feature level 0x%x", level);
            break;
        }
    }

    if (!id3d12device) {
        LOG_ERROR(L"Failed to create D3D12 device.");
        throw std::runtime_error("Failed to create D3D12 device.");
    }

    enableDeviceDebugLayer(id3d12device);

    return id3d12device;
}

void Device::enableDeviceDebugLayer(ComPtr<ID3D12Device2>& debugDevice) {
    #if defined(_DEBUG)
        ComPtr<ID3D12InfoQueue> infoQueue;
        
        if (SUCCEEDED(debugDevice.As(&infoQueue))) {
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

            D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
            D3D12_MESSAGE_ID denyIds[] = {
                D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            };

            D3D12_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumSeverities = _countof(severities);
            filter.DenyList.pSeverityList = severities;
            filter.DenyList.NumIDs = _countof(denyIds);
            filter.DenyList.pIDList = denyIds;

            infoQueue->PushStorageFilter(&filter);
        }
    #endif
}

void Device::enableDebugLayer() {
    #if defined(_DEBUG)
        ComPtr<ID3D12Debug> debugController;

        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
        }
    #endif
}