#pragma once
#include "raylib.h"
#include "Systems.hpp"


class RenderingSystem : Koji::Systems::System
{
public:
    RenderingSystem(uint16_t width, uint16_t height, const char* title);
    ~RenderingSystem() override;

    bool Frame      (entt::registry registry)   override;
    bool BeginFrame (entt::registry registry)   override;
    bool EndFrame   (entt::registry registry)   override;

    void DrawMesh(const Mesh& mesh, const Transform transform, const Material material); // TODO implement mesh drawing

    Camera3D& GetCamera() { return camera; }

private:
    Camera3D camera;
    uint16_t width, height;
};
