#pragma once

#include "utils/pch.h"

class Device
{
public:
    Device(bool useWarp);
    ~Device() = default;

    void initialize(bool useWarp = false);

    ComPtr<IDXGIAdapter4> selectAdapter(bool useWarp);
    ComPtr<ID3D12Device2> createDevice(ComPtr<IDXGIAdapter4> deviceAdapter);

    void enableDebugLayer();
    void enableDeviceDebugLayer(ComPtr<ID3D12Device2> &debugDevice);

    bool checkForTearingSupport();

    // getters
    ComPtr<ID3D12Device2> getDevice() const
    {
        return device;
    }

    ComPtr<IDXGIAdapter4> getAdapter() const
    {
        return adapter;
    }

    bool getSupportTearingState() const
    {
        return supportTearing;
    }

private:
    ComPtr<IDXGIAdapter4> adapter;
    ComPtr<ID3D12Device2> device;

    bool supportTearing = false;
};