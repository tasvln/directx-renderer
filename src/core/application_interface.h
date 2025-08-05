#pragma once

#include "utils/pch.h"
#include "utils/events.h"
#include "window.h"

// forward declaration
class Window;

class IApplication : public std::enable_shared_from_this<IApplication> {
    public:
        /**
         * Create the DirectX demo using the specified window dimensions.
         */
        IApplication(const std::wstring& name, int width, int height, bool vSync);
        virtual ~IApplication();

        /**
         *  Initialize the DirectX Runtime.
         */
        virtual bool initialize();
    
        /**
         *  Load content required for the demo.
         */
        virtual bool loadContent() = 0;
    
        /**
         *  Unload demo specific content that was loaded in LoadContent.
         */
        virtual void unloadContent() = 0;
    
        /**
         * Destroy any resource that are used by the game.
         */
        virtual void destroy();

        int getWidth() const {
            return width;
        }

        int getHeight() const {
            return height;
        }
        
    protected:
        friend class Window;
    
        /**
         *  Update the game logic.
         */
        virtual void onUpdate(UpdateEventArgs& e);
    
        /**
         *  Render stuff.
         */
        virtual void onRender(RenderEventArgs& e);
    
        /**
         * Invoked by the registered window when a key is pressed
         * while the window has focus.
         */
        virtual void onKeyPressed(KeyEventArgs& e);
    
        /**
         * Invoked when a key on the keyboard is released.
         */
        virtual void onKeyReleased(KeyEventArgs& e);
    
        /**
         * Invoked when the mouse is moved over the registered window.
         */
        virtual void onMouseMoved(MouseMotionEventArgs& e);
    
        /**
         * Invoked when a mouse button is pressed over the registered window.
         */
        virtual void onMouseButtonPressed(MouseButtonEventArgs& e);
    
        /**
         * Invoked when a mouse button is released over the registered window.
         */
        virtual void onMouseButtonReleased(MouseButtonEventArgs& e);
    
        /**
         * Invoked when the mouse wheel is scrolled while the registered window has focus.
         */
        virtual void onMouseWheel(MouseWheelEventArgs& e);
    
        /**
         * Invoked when the attached window is resized.
         */
        virtual void onResize(ResizeEventArgs& e);
    
        /**
         * Invoked when the registered window instance is destroyed.
         */
        virtual void onWindowDestroy();

        std::shared_ptr<Window> pWindow;
        
    private:
        std::wstring name;
        int width;
        int height;
        bool vsync;
};