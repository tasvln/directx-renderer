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

#define LOG_INFO(fmt, ...)    Logger::instance().log(LogType::Info,    __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) Logger::instance().log(LogType::Warning, __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)   Logger::instance().log(LogType::Error,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   Logger::instance().log(LogType::Debug,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)

static const UINT FRAMEBUFFERCOUNT = 3;

using namespace Microsoft::WRL;
using namespace DirectX;

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

inline void throwFailed(HRESULT hr) {
    if (FAILED(hr)) {
        LOG_ERROR(L"throwFailed: HRESULT = 0x%08X", hr);
        throw std::runtime_error("HRESULT failed");
    }
}

