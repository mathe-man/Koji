#include <Koji/Core/Application.h>
#include "Koji//ECS/Systems/Systems.hpp"
#include "Koji/ECS/Components/Components.hpp"
#include <raylib.h>

#include "imgui.h"
#include "imgui_impl_raylib.h"

using namespace Koji::Components;
using namespace Koji::Systems;
using namespace Koji::Core;

System::System(const ApplicationData& data)
{ }


RenderingSystem::RenderingSystem(const ApplicationData& data) : System(data)
{
    

    InitWindow(data.window_width, data.window_height, data.name);
    SetTargetFPS(60);

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f ,0.0f ,0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    ImGui::CreateContext();
    ImGui_ImplRaylib_Init();
}


bool RenderingSystem::BeginFrame(entt::registry &registry)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    BeginMode3D(camera);

    
    ImGui_ImplRaylib_NewFrame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow();

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
    ImGui::Render();
    ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());

    EndMode3D();
    EndDrawing();

    return true;
}


