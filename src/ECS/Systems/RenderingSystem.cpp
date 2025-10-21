#include "Koji//ECS/Systems/Systems.hpp"
#include "Koji/ECS/Components/Components.hpp"
#include <raylib.h>

using namespace Koji::Components;
using namespace Koji::Systems;

System::System()
{ }


RenderingSystem::RenderingSystem(uint16_t width, uint16_t height, const char* title)
{
    this->width = width;
    this->height = height;

    InitWindow(this->width, this->height, title);
    SetTargetFPS(60);

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f ,0.0f ,0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // ImGui::CreateContext();
    // ImGui_ImplRaylib_Init();
}


bool RenderingSystem::BeginFrame(entt::registry &registry)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    // TODO add imgui and call those methods
    // ImGui_ImplRaylib_NewFrame();
    // ImGui::NewFrame();

    auto view = registry.view<kTransform, Mesh, Material>();
    for (auto entity : view) {
        auto& trans = view.get<kTransform>(entity);
        auto& mesh = view.get<Mesh>(entity);
        auto& mat = view.get<Material>(entity);

        //DrawMesh(mesh, trans, mat);
    }
    
    return true;
}



bool RenderingSystem::EndFrame(entt::registry &registry)
{
    // ImGui::Render();
    //     ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

    EndMode3D();
    EndDrawing();

    return true;
}


