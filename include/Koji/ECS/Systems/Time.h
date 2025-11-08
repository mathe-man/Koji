#pragma once
#include <chrono>
#include "../System.h"

namespace Koji::ECS
{
    // TODO add a scoped timer (trigger function when the scope exit)
    // TODO add a delay timer (trigger function when the delay is reached)
    class TimeSystem : public System
    {
        using clock = std::chrono::high_resolution_clock;
    
    public:
        explicit TimeSystem() = default;
        ~TimeSystem() override = default;

        bool Init() override {
            start_time = clock::now();
            return true;
        }
        bool Update ()   override {
            auto now = clock::now();
            std::chrono::duration<float> delta = now - last_frame;
            deltaTime = delta.count();
            last_frame = now;

            // add #include <iostream> to use this test
            // std::cout << "DeltaTime: " << deltaTime << " s | FPS: " << GetFPS() << "\n";
            
            return true;
        }
    

        [[nodiscard]] float GetDeltaTime() const { return deltaTime; }
        [[nodiscard]] float GetFPS() const { return 1/deltaTime; }
    private:
        clock::time_point start_time;
        clock::time_point last_frame;
        float deltaTime{};
    };
}