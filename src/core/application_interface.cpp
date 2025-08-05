#include "application_interface.h"

bool IApplication::initialize() {
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBoxA(NULL, "Failed to verify DirectX Math library support.", "Error", MB_OK | MB_ICONERROR);
        
        LOG_ERROR(L"Failed to verify DirectX Math library support, Application Initialization Failed");
        return false;
    }
 
    pWindow = Engine::getEngine().createRenderWindow(
        name,
        width,
        height,
        vsync
    );
    
    pWindow->registerCallbacks(shared_from_this());
    pWindow->show();
 
    return true;
}

void IApplication::destroy()
{
    Engine::getEngine().destroyWindow(pWindow);
    pWindow.reset();
}

void IApplication::onUpdate(UpdateEventArgs& e) {}

void IApplication::onRender(RenderEventArgs& e) {}

void IApplication::onKeyPressed(KeyEventArgs& e) {}

void IApplication::onKeyReleased(KeyEventArgs& e) {}

void IApplication::onMouseMoved(MouseMotionEventArgs& e) {}

void IApplication::onMouseButtonPressed(MouseButtonEventArgs& e) {}

void IApplication::onMouseButtonReleased(MouseButtonEventArgs& e) {}

void IApplication::onMouseWheel(MouseWheelEventArgs& e) {}

void IApplication::onResize(ResizeEventArgs& e) {
    width = e.width;
    height = e.height;
}

void IApplication::onWindowDestroy() {
    // If the Window which we are registered to is 
    // destroyed, then any resources which are associated 
    // to the window must be released.
    unloadContent();
}