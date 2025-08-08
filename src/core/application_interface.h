#pragma once

#include "utils/pch.h"
#include "utils/events.h"
#include "engine/engine.h"

// forward declaration
class Engine;

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
        std::shared_ptr<Engine> engine;

    private:
        std::wstring name;
        int width;
        int height;
        bool vsync;
};