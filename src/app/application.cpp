#include "application.h"
#include "window.h"

Application::Application(
    HINSTANCE hInstance, 
    WindowConfig &config
) :  
    config(config) 
{
    window = std::make_unique<Window>(hInstance, config, this);
}

void Application::init() {
    LOG_INFO(L"Initializing Engine Classes");
}

int Application::run() {
    MSG msg = {};

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // else
        // {
        //     onUpdate();
        //     onRender();
        // }
    }

    return static_cast<int>(msg.wParam);
}

void Application::onResize(UINT width, UINT height)
{
    LOG_INFO(L"Application resize: %dx%d", width, height);
    // Resize swapchain & depth buffer here
}

void Application::onUpdate()
{
    // Game/update logic
}

void Application::onRender()
{
    // Issue rendering commands
}