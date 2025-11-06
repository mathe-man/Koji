#include <Koji/Scene.h>
#include <Koji/Systems.hpp>
#include <Koji/Components.hpp>

#include <raylib.h>

using namespace Koji::Engine;
using namespace Koji::Engine::Components;




bool RaylibRenderingSystem::Init(const Scene& scene, entt::registry& registry)
{
    // Raylib setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(scene.window_width, scene.window_height, scene.name);
    SetTargetFPS(60);

    
    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f ,0.0f ,0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    return true;
}

bool RaylibRenderingSystem::BeginFrame(entt::registry &registry)
{
    if (WindowShouldClose())
        return false;
    
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    
    auto view = registry.view<kTransform, kSphere>();
    for (auto entity : view) {
        auto& trans = view.get<kTransform>(entity);
        auto& sphere = view.get<kSphere>(entity);
        DrawSphereWires(trans.position, sphere.radius, sphere.rings, sphere.slices, sphere.color);
    }

    EndMode3D(); // 3D mode need to be ended before starting to use ImGui
    EndDrawing();
    
    return true;
}


bool RaylibRenderingSystem::Close()
{
    CloseWindow();
    return true;
}


