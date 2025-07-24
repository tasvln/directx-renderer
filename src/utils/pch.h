#pragma once

#ifdef WIN32
    #include <windows.h>
#endif

#include <shellapi.h>
#include <d3d12.h>

#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>

#include "logger.h"

#define LOG_INFO(fmt, ...)    Logger::instance().log(LogType::Info,    __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_WARNING(fmt, ...) Logger::instance().log(LogType::Warning, __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...)   Logger::instance().log(LogType::Error,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   Logger::instance().log(LogType::Debug,   __FILE__, __FUNCTION__, __LINE__, fmt, __VA_ARGS__)

// future -> can put this in a settings file if i decide on a debug UI
struct WindowConfig {
    LPCWSTR appName;
    LPCWSTR windowClassName;
    INT width;
    INT height;
    // bool vsync = true;
    // bool fullscreen = false;
    // bool resizable = true;
};