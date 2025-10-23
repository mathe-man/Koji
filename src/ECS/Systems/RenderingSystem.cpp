#include <Koji/Core/Application.h>
#include "Koji//ECS/Systems/Systems.hpp"
#include "Koji/ECS/Components/Components.hpp"
#include <raylib.h>

#include "imgui.h"
#include "rlImGui.h"

using namespace Koji::Components;
using namespace Koji::Systems;
using namespace Koji::Core;

System::System(const ApplicationData& data)
{ }


RenderingSystem::RenderingSystem(const ApplicationData& data) : System(data)
{

    // Raylib setup
    InitWindow(data.window_width, data.window_height, data.name);
    SetTargetFPS(60);
    // ImGui setup
    rlImGuiSetup(true);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable; // Flags

    camera.position = { 0.0f, 10.0f, 10.0f };
    camera.target   = { 0.0f ,0.0f ,0.0f };
    camera.up       = { 0.0f, 1.0f, 0.0f };
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

}

bool RenderingSystem::BeginFrame(entt::registry &registry)
{
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

    // === ImGui ===
    rlImGuiBegin();
    CreateMainDockspace();

    ImGui::ShowDemoWindow();

    ImGui::Begin("Some window");
    ImGui::Text("Some text");
    ImGui::End();
    
    return true;
}




bool RenderingSystem::EndFrame(entt::registry &registry)
{
    rlImGuiEnd();
    EndDrawing();

    return true;
}


void RenderingSystem::CreateMainDockspace()
{
    
    // Get Raylib window size
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    // Flags to make this window entirely invisible
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    // Remove border effects
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("Main dockspace window", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    // Create dockspace
    ImGuiID dockspace_id = ImGui::GetID("Main dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
}
