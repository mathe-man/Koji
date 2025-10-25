#include <Koji/Engine/Scene.h>
#include <Koji/Engine/Systems.hpp>
#include "Koji/Engine/Components.hpp"

#include <raylib.h>
#include "imgui.h"
#include "rlImGui.h"

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
    
    auto view = registry.view<kTransform, Mesh, Material>();
    for (auto entity : view) {
        auto& trans = view.get<kTransform>(entity);
        auto& mesh = view.get<Mesh>(entity);
        auto& mat = view.get<Material>(entity);

        //DrawMesh(mesh, trans, mat);
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


