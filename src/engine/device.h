#pragma once

#include "utils/pch.h"

class Device {
    public:
        Device() = default;
        ~Device() = default;
    
        void initialize(bool useWarp = false);

        ComPtr<IDXGIAdapter4> selectAdapter(bool useWarp);
        ComPtr<ID3D12Device2> createDevice(ComPtr<IDXGIAdapter4> deviceAdapter);

        void enableDebugLayer();
        void enableDeviceDebugLayer(ComPtr<ID3D12Device2>& debugDevice);

        // getters
        ComPtr<ID3D12Device2> getDevice() const { 
            return device; 
        }

        ComPtr<IDXGIAdapter4> getAdapter() const { 
            return adapter; 
        }

    private:
        ComPtr<IDXGIAdapter4> adapter;
        ComPtr<ID3D12Device2> device;

};