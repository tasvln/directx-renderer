#pragma once

class IEngine {
    public:
        IEngine() = default;
        
        virtual ~IEngine() = default;

        virtual void init() = 0;

        virtual void update() = 0;
    protected:
    
    private:

};