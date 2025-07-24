#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <shellapi.h>
#include <d3d12.h>

#include <iostream>
#include <memory>

struct WindowConfig {
    LPCWSTR appName;
    LPCWSTR windowClassName;
    INT width;
    INT height;
};