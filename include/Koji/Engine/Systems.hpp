#pragma once
#include <cstdint>
#include <raylib.h>

// forward declarations
namespace entt
{ class registry; }

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
   


class RenderingSystem : public System
{
    public:
        ~RenderingSystem() override = default;

        [[nodiscard]] const char* GetName() const override { return "Rendering System"; }
        bool Init(const Scene& scene, entt::registry& registry) override;

        bool Frame      (entt::registry &registry)   override { return true; }
        bool BeginFrame (entt::registry &registry)   override;
        bool EndFrame   (entt::registry &registry)   override;

        bool Close() override;

        Camera3D& GetCamera() { return camera; }

    private:
        Camera3D camera {};
        uint16_t width {}, height {};

        void CreateMainDockspace();
};


} // namespace Koji::Engine
