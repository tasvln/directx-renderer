#pragma once

#include "utils/pch.h"
#include "utils/events.h"
#include "utils/event_interface.h"
#include "engine/engine.h"

// forward declaration
class Engine;

class IApplication : public IWindowEventHandler, public std::enable_shared_from_this<IApplication>
{
public:
    /**
     * Create the DirectX demo using the specified window dimensions.
     */
    IApplication(WindowConfig& config);
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

    int getWidth() const
    {
        return config.width;
    }

    int getHeight() const
    {
        return config.height;
    }

    void setWidth(int width)
    {
        this->config.width = width;
    }

    void setHeight(int height)
    {
        this->config.height = height;
    }

protected:
    std::shared_ptr<Engine> engine;

private:
    WindowConfig& config;
};