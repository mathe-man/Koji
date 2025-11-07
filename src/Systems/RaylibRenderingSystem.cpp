#include <Koji/Scene.h>
#include <Koji/Systems.hpp>
#include <Koji/ECS/Components/Sphere.h>

#include <raylib.h>

#include "Koji/Application.h"
#include "Koji/ECS/Chunck.h"

using namespace Koji::Engine;




bool RaylibRenderingSystem::Init(const Scene& scene)
{
    // Raylib setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, scene.name.c_str());
    SetTargetFPS(60);

    
    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f ,0.0f ,0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    return true;
}

bool RaylibRenderingSystem::BeginFrame()
{
    if (WindowShouldClose())
        return false;
    
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    
    
    auto archetypes = Application::scene->world.Query({kTransform::TypeId, MetaData::TypeId});
    
    for (auto archetype : archetypes)
        for (auto chunk : archetype->chunks)
            for (size_t i = 0; i < chunk->count; ++i)
            {
                // Get the array of each component
                auto* t = static_cast<kTransform*>(
                    chunk->GetComponentArray(kComponent<kTransform>::TypeId)
                );
                auto* s = static_cast<kSphere*>(
                    chunk->GetComponentArray(kComponent<kSphere>::TypeId)
                );
                
                // Get the component for the number i entity
                kTransform& trans = t[i];
                kSphere& sphere = s[i];
                
                // Do stuff with infos
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


