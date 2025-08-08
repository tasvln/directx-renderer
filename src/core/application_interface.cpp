#include "application_interface.h"

IApplication::IApplication(
    const std::wstring& name, 
    int width, 
    int height, 
    bool vSync
) : 
    name(name), 
    width(width), 
    height(height), 
    vsync(vSync) 
{}

bool IApplication::initialize(std::shared_ptr<Engine> engine) {
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBox(
            NULL, 
            L"Failed to verify DirectX Math library support.", 
            L"Error",
            MB_OK | MB_ICONERROR
        );
        LOG_ERROR(L"Failed to verify DirectX Math library support, Application Initialization Failed");
        return false;
    }
 
    // Create the window through the engine
    engine = Engine::getEnginePtr();
    auto window = engine->createRenderWindow(name, width, height, vsync);
    
    // Register this object as the event handler
    window->registerEventHandler(shared_from_this());

    window->show();
 
    return true;
}

void IApplication::destroy()
{
    if (engine) {
        engine->destroyAllWindows();
        engine.reset();
    }
}

// void IApplication::onUpdate(UpdateEventArgs& e) {}

// void IApplication::onRender(RenderEventArgs& e) {}

// void IApplication::onKeyPressed(KeyEventArgs& e) {}

// void IApplication::onKeyReleased(KeyEventArgs& e) {}

// void IApplication::onMouseMoved(MouseMotionEventArgs& e) {}

// void IApplication::onMouseButtonPressed(MouseButtonEventArgs& e) {}

// void IApplication::onMouseButtonReleased(MouseButtonEventArgs& e) {}

// void IApplication::onMouseWheel(MouseWheelEventArgs& e) {}

// void IApplication::onResize(ResizeEventArgs& e) {
//     width = e.width;
//     height = e.height;
// }

// void IApplication::onWindowDestroy() {
//     // If the Window which we are registered to is 
//     // destroyed, then any resources which are associated 
//     // to the window must be released.
//     unloadContent();
// }