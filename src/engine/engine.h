#pragma once

#include "utils/engine_interface.h"

class Engine: public IEngine {
    public:
        Engine();
        
        ~Engine();

        void init() override;

        void update() override;

    private:

};