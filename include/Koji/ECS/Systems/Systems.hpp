#pragma once
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <entt/entt.hpp>
#include <raylib.h>

namespace Koji::Core
{
    struct ApplicationData;
}

namespace Koji::Systems
{
    
class System
{
    public:
        explicit System();
        virtual ~System() = default;

        virtual bool Init(const Core::ApplicationData& data, entt::registry& registry) = 0;
    
        virtual bool Frame      (entt::registry &registry)   = 0;
        virtual bool BeginFrame (entt::registry &registry)   = 0;
        virtual bool EndFrame   (entt::registry &registry)   = 0;
};
   


class RenderingSystem : public System
{
    public:
        RenderingSystem();
        ~RenderingSystem() override = default;

        bool Init(const Core::ApplicationData& data, entt::registry& registry) override;

        bool Frame      (entt::registry &registry)   override { return true; }
        bool BeginFrame (entt::registry &registry)   override;
        bool EndFrame   (entt::registry &registry)   override;


        Camera3D& GetCamera() { return camera; }

    private:
        Camera3D camera;
        uint16_t width, height;

        void CreateMainDockspace();
};


}
