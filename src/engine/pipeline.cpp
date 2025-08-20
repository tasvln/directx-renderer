#include "pipeline.h"

Pipeline::Pipeline(
    ComPtr<ID3D12Device2> device,
    const Shader& vertexShader,
    const Shader& pixelShader,
    const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout,
    const std::vector<D3D12_ROOT_PARAMETER>& rootParams,
    const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers,
    DXGI_FORMAT rtvFormat,
    DXGI_FORMAT dsvFormat
) {
    LOG_INFO(L"Starting Pipeline creation...");

    // Root signature
    D3D12_ROOT_SIGNATURE_DESC rootDesc = {};
    rootDesc.NumParameters = static_cast<UINT>(rootParams.size());
    rootDesc.pParameters = rootParams.empty() ? nullptr : rootParams.data();
    rootDesc.NumStaticSamplers = static_cast<UINT>(samplers.size());
    rootDesc.pStaticSamplers = samplers.empty() ? nullptr : samplers.data();
    rootDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> serializedRootSig;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(
        &rootDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &serializedRootSig,
        &errorBlob
    );
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            LOG_ERROR(L"Root signature serialization error: %S", (char*)errorBlob->GetBufferPointer());
        } else {
            LOG_ERROR(L"Failed to serialize root signature without error blob.");
        }
        throwFailed(hr);
    }
    LOG_INFO(L"Root signature serialized successfully.");

    hr = device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)
    );
    throwFailed(hr);
    LOG_INFO(L"Root signature created successfully.");

    // PSO description
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    LOG_INFO(L"PSO -> Initialized D3D12_GRAPHICS_PIPELINE_STATE_DESC struct");

    psoDesc.InputLayout = { 
        inputLayout.data(), 
        static_cast<UINT>(inputLayout.size()) 
    };
    LOG_INFO(L"PSO -> InputLayout set with %d elements", static_cast<UINT>(inputLayout.size()));

    psoDesc.pRootSignature = rootSignature.Get();
    LOG_INFO(L"PSO -> RootSignature assigned");

    psoDesc.VS = { 
        vertexShader.getBytecode()->GetBufferPointer(), 
        vertexShader.getBytecode()->GetBufferSize() 
    };
    LOG_INFO(L"PSO -> Vertex shader bytecode set, size: %llu bytes", vertexShader.getBytecode()->GetBufferSize());

    psoDesc.PS = { 
        pixelShader.getBytecode()->GetBufferPointer(), 
        pixelShader.getBytecode()->GetBufferSize() 
    };
    LOG_INFO(L"PSO -> Pixel shader bytecode set, size: %llu bytes", pixelShader.getBytecode()->GetBufferSize());

    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    LOG_INFO(L"PSO -> RasterizerState set to default");

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    LOG_INFO(L"PSO -> BlendState set to default");

    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    LOG_INFO(L"PSO -> DepthStencilState set to default");

    psoDesc.SampleMask = UINT_MAX;
    LOG_INFO(L"PSO -> SampleMask set to UINT_MAX");

    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    LOG_INFO(L"PSO -> PrimitiveTopologyType set to TRIANGLE");

    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = rtvFormat;
    LOG_INFO(L"PSO -> Number of Render Targets set to %d, RTVFormat set", psoDesc.NumRenderTargets);

    psoDesc.DSVFormat = dsvFormat;
    LOG_INFO(L"PSO -> DSVFormat set");

    psoDesc.SampleDesc.Count = 1;
    LOG_INFO(L"PSO -> SampleDesc.Count set to %d", psoDesc.SampleDesc.Count);

    hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
    if (FAILED(hr)) {
        LOG_ERROR(L"Failed to create PSO. HRESULT: 0x%08X", hr);
        throwFailed(hr);
    } else {
        LOG_INFO(L"Graphics pipeline state created successfully");
    }

    LOG_INFO(L"Pipeline creation finished");
}
