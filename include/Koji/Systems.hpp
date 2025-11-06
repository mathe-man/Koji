#pragma once
#include <chrono>
#include <cstdint>
#include <raylib.h>
#include <entt/entt.hpp>


namespace Koji::Engine
{
// forward declaration
struct Scene;
    
    
class System
{
    public:
        explicit System() = default;
        virtual ~System() = default;

        [[nodiscard]] virtual const char* GetName() const    = 0;
        virtual bool Init(const Scene& scene, entt::registry& registry) = 0;

        virtual bool Frame      (entt::registry &registry)   = 0;
        virtual bool BeginFrame (entt::registry &registry)   = 0;
        virtual bool EndFrame   (entt::registry &registry)   = 0;

        virtual bool Close() = 0;
};
   


class RaylibRenderingSystem : public System
{
    public:
        ~RaylibRenderingSystem() override = default;

        [[nodiscard]] const char* GetName() const override { return "Raylib Rendering System"; }
        bool Init(const Scene& scene, entt::registry& registry) override;

        bool Frame      (entt::registry &registry)   override { return true; }
        bool BeginFrame (entt::registry &registry)   override;
        bool EndFrame   (entt::registry &registry)   override { return true; }

        bool Close() override;

        Camera3D& GetCamera() { return camera; }

    private:
        Camera3D camera {};
        uint16_t width {}, height {};
};


// TODO add in this system the functionnalities present in the Time class from Time.h
class TimeSystem : public System
{
    using clock = std::chrono::high_resolution_clock;
    
    public:
        explicit TimeSystem() = default;
        ~TimeSystem() override = default;

        [[nodiscard]] const char* GetName() const override { return "Time System"; }
        bool Init(const Scene& scene, entt::registry& registry) override {
            start_time = clock::now();
            return true;
        }

        bool Frame      (entt::registry &registry)   override { return true; }
        bool BeginFrame (entt::registry &registry)   override {
            auto now = clock::now();
            std::chrono::duration<float> delta = now - last_frame;
            deltaTime = delta.count();
            last_frame = now;

            // add #include <iostream> to use this test
            // std::cout << "DeltaTime: " << deltaTime << " s | FPS: " << GetFPS() << "\n";
            
            return true;
        }
        bool EndFrame   (entt::registry &registry)   override { return true; }
    
        bool Close() override { return true; }


        [[nodiscard]] float GetDeltaTime() const { return deltaTime; }
        [[nodiscard]] float GetFPS() const { return 1/deltaTime; }
    private:
        clock::time_point start_time;
        clock::time_point last_frame;
        float deltaTime{};
};

} // namespace Koji::Engine
