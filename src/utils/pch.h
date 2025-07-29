#pragma once

#define NOMINMAX
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <shellapi.h>
#include <wrl.h>

#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <directx/d3dx12.h>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <memory>
#include <filesystem>

#include <string>
#include <fstream>

#include "logger.h"
#include "helper.h"

#define LOG_INFO(fmt, ...)    Logger::instance().log(LogType::Info,    __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) Logger::instance().log(LogType::Warning, __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)   Logger::instance().log(LogType::Error,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   Logger::instance().log(LogType::Debug,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)

#define FRAMEBUFFERCOUNT 3

using namespace Microsoft::WRL;

// future -> can put this in a settings file if i decide on a debug UI
struct WindowConfig {
    LPCWSTR appName;
    LPCWSTR windowClassName;
    uint32_t width;
    uint32_t height;
    bool enabledDirectX;
    bool useWarp;
    bool vsync = true;
    bool fullscreen = false;
    bool resizable = true;
};

// Window handle.
// HWND g_hWnd;
// // Window rectangle (used to toggle fullscreen state).
// RECT g_WindowRect;

// // DirectX 12 Objects
// ComPtr<ID3D12Device2> g_Device;
// ComPtr<ID3D12CommandQueue> g_CommandQueue;
// ComPtr<IDXGISwapChain4> g_SwapChain;
// ComPtr<ID3D12Resource> g_BackBuffers[FRAMEBUFFERCOUNT];
// ComPtr<ID3D12GraphicsCommandList> g_CommandList;
// ComPtr<ID3D12CommandAllocator> g_CommandAllocators[FRAMEBUFFERCOUNT];
// ComPtr<ID3D12DescriptorHeap> g_RTVDescriptorHeap;
// UINT g_RTVDescriptorSize;
// UINT g_CurrentBackBufferIndex;

// Synchronization objects
// ComPtr<ID3D12Fence> g_Fence;
// uint64_t g_FenceValue = 0;
// uint64_t g_FrameFenceValues[g_NumFrames] = {};
// HANDLE g_FenceEvent;