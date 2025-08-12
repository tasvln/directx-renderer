#pragma once

#include "utils/pch.h"
#include <unordered_map>

class WindowManager
{
public:
    WindowManager(HINSTANCE hInstance);
    ~WindowManager() = default;

    std::shared_ptr<Window> createWindow(const std::wstring &name, const WindowConfig &config);
    std::shared_ptr<Window> getWindow(HWND hwnd);
    std::shared_ptr<Window> getWindow(const std::wstring &name);

    void destroyWindow(const std::wstring &name);
    void destroyAll();

private:
    HINSTANCE hInstance;
    std::unordered_map<HWND, std::shared_ptr<Window>> windowsByHandle;
    std::unordered_map<std::wstring, std::shared_ptr<Window>> windowsByName;
};
