#include "window_manger.h"
#include "window.h"

std::shared_ptr<Window> WindowManager::createWindow(const std::wstring &name, const WindowConfig &config)
{
    // Prevent duplicate windows by name.
    auto it = windowsByName.find(name);
    if (it != windowsByName.end())
    {
        return it->second;
    }

    // Create a new window instance.
    auto window = std::make_shared<Window>(config.hInstance, const_cast<WindowConfig &>(config));

    // Store in maps.
    windowsByHandle[window->getHwnd()] = window;
    windowsByName[name] = window;

    return window;
}

std::shared_ptr<Window> WindowManager::getWindow(HWND hwnd)
{
    auto it = windowsByHandle.find(hwnd);
    return (it != windowsByHandle.end()) ? it->second : nullptr;
}

std::shared_ptr<Window> WindowManager::getWindow(const std::wstring &name)
{
    auto it = windowsByName.find(name);
    return (it != windowsByName.end()) ? it->second : nullptr;
}

void WindowManager::destroyWindow(const std::wstring &name)
{
    auto it = windowsByName.find(name);
    if (it != windowsByName.end())
    {
        HWND hwnd = it->second->getHwnd();
        DestroyWindow(hwnd);

        windowsByHandle.erase(hwnd);
        windowsByName.erase(it);
    }
}

void WindowManager::destroyAll()
{
    for (auto &pair : windowsByHandle)
    {
        DestroyWindow(pair.first);
    }
    windowsByHandle.clear();
    windowsByName.clear();
}

// Singleton access (optional)
WindowManager &WindowManager::getInstance()
{
    static WindowManager instance;
    return instance;
}
