#pragma once
#include <Koji/Engine/Systems.hpp>

#include <cstdint>

namespace Koji::Editor::Systems
{

class EditorRenderingSystem : public Engine::System
{
public:
    ~EditorRenderingSystem() override = default;

    const char* GetName() const override { return "Editor's Rendering System"; }
    bool Init(const Engine::Scene& scene, entt::registry& registry) override;

    bool BeginFrame (entt::registry& registry)  override;

    bool Frame(entt::registry& registry) override { return true; }
    bool EndFrame(entt::registry& registry) override { return true; }

    bool Close() override;

    Camera3D& GetCamera() { return camera; }

    RenderTexture2D render_target {};

private:
    Camera3D camera {};
    int lastTexW = 0;
    int lastTexH = 0;
};
    
} // namespace Koji::Editor::Systems