#pragma once
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <entt/entt.hpp>
#include <raylib.h>

namespace Koji::Systems
{
    
class System
{
    public:
        System();
        virtual ~System() = default;
        
        virtual bool Frame      (entt::registry &registry)   = 0;
        virtual bool BeginFrame (entt::registry &registry)   = 0;
        virtual bool EndFrame   (entt::registry &registry)   = 0;
};
   


class RenderingSystem : public System
{
    public:
        RenderingSystem(uint16_t width, uint16_t height, const char* title);
        ~RenderingSystem() override = default;

        bool Frame      (entt::registry &registry)   override { return true; }
        bool BeginFrame (entt::registry &registry)   override;
        bool EndFrame   (entt::registry &registry)   override;


        Camera3D& GetCamera() { return camera; }

    private:
        Camera3D camera;
        uint16_t width, height;
};


}