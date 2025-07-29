#include "frame_timer.h"
#include <sstream>

FrameTimer::FrameTimer() {
    previousTime = std::chrono::high_resolution_clock::now();
}

void FrameTimer::tick() {
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frameDuration = currentTime - previousTime;
    previousTime = currentTime;

    deltaTime = frameDuration.count();
    elapsedTime += deltaTime;
    frameCount++;

    if (elapsedTime >= 1.0) {
        fps = frameCount / elapsedTime;
        frameCount = 0;
        elapsedTime = 0.0;
    }
}

double FrameTimer::getDeltaTime() const {
    return deltaTime;
}

double FrameTimer::getFPS() const {
    return fps;
}

std::wstring FrameTimer::getFPSString() const {
    std::wstringstream ss;
    ss.precision(2);
    ss << std::fixed << L"FPS: " << fps;
    return ss.str();
}