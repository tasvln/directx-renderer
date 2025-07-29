#pragma once

#include <chrono>
#include <string>

class FrameTimer {
    public:
        FrameTimer();
        ~FrameTimer() = default;

        void tick();

        double getDeltaTime() const;
        double getFPS() const;
        std::wstring getFPSString() const;

    private:
        std::chrono::high_resolution_clock::time_point previousTime{};
        double deltaTime = 0.0;
        double elapsedTime = 0.0;
        int frameCount = 0;
        double fps = 0.0;
};