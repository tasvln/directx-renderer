#pragma once

#include "utils/pch.h"

class Device
{
    public:
        Device(bool useWarp);
        ~Device() = default;

        ComPtr<ID3D12Device2> GetDevice() const { 
            return device; 
        }

        ComPtr<IDXGIAdapter4> GetAdapter() const { 
            return adapter; 
        }

        bool getSupportTearingState() const { 
            return supportTearing; 
        }

        D3D_FEATURE_LEVEL GetFeatureLevel() const { 
            return featureLevel; 
        }

        ComPtr<ID3D12Device2> getDevice() const {
            return device;
        }

    private:
        void enableDebugLayer();
        ComPtr<IDXGIAdapter4> selectAdapter(bool useWarp);
        ComPtr<ID3D12Device2> createDevice(ComPtr<IDXGIAdapter4> adapter);
        bool checkForTearingSupport();

    private:
        ComPtr<IDXGIAdapter4> adapter;
        ComPtr<ID3D12Device2> device;
        
        bool supportTearing = false;
        UINT dxgiFactoryFlags = 0;

        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};