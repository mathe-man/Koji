#include <Koji/Editor/Systems.hpp>

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#include "Koji/Editor/GuiManager.hpp"
#include "Koji/Engine/Components.hpp"
#include "Koji/Engine/Scene.h"
#include "reactphysics3d/engine/OverlappingPairs.h"

using namespace Koji::Engine;
using namespace Koji::Editor;
using namespace Koji::Editor::Systems;

bool EditorRenderingSystem::Init(const Scene& scene, entt::registry& registry)
{
    // Raylib setup
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(scene.window_width, scene.window_height, scene.name);
    render_target = LoadRenderTexture(scene.window_width, scene.window_height);
    SetTextureFilter(render_target.texture, TEXTURE_FILTER_BILINEAR);

    // Variables for ImGui window size tracking
    lastTexW = render_target.texture.width;
    lastTexH = render_target.texture.height;
    
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
    
    return true && GuiManager::Init(registry);
}

bool EditorRenderingSystem::BeginFrame(entt::registry& registry)
{
    if (WindowShouldClose())
        return false;

    // === Raylib ===
    BeginDrawing();
    ClearBackground(DARKGRAY);

    // 3D drawing of the scene sur  
    BeginTextureMode(render_target);
    ClearBackground(RAYWHITE);
    // Game view drawing => will be displayed in a ImGui window after
    BeginMode3D(camera);

    DrawSphereWires({0.f, 0.f, 0.f}, 2.0f, 10, 15, RED);
    DrawCubeWiresV(Vector3{0.f,0.f,0.f}, Vector3{3.f,3.f,3.f}, BLUE);
    
    auto view = registry.view<Components::kTransform, Mesh, Material>();
    for (auto entity : view) {
        // TODO add mesh and material components
        /*
        auto& trans = view.get<Components::kTransform>(entity);
        auto& trans = view.get<Mesh>(entity);
        auto& trans = view.get<Material>(entity);

        DrawMes(mesh, trans, mat)
        */
    }


    EndMode3D();
    EndTextureMode();

    // === ImGui / Editor Ui ===
    rlImGuiBegin();
    

    // TODO make a 3D scene window from this code
    /*
    ImGui::Begin("Scene - 3D render");

    // Allow the ImGui window user to request a particular viewport size (or use content region size)
    ImVec2 avail = ImGui::GetContentRegionAvail();

    // If the content region changed, resize the render texture to match (or scale as you want)
    int wantW = (int)avail.x;
    int wantH = (int)avail.y;

    if (wantW <= 0) wantW = 64;
    if (wantH <= 0) wantH = 64;

    if (wantW != lastTexW || wantH != lastTexH) {
        UnloadRenderTexture(render_target);
        render_target = LoadRenderTexture(wantW, wantH);
        lastTexW = wantW;
        lastTexH = wantH;
    }

    // Convert from Raylib/OpenGl texture id -> ImGui texture id
    ImTextureID texId = (ImTextureID)(intptr_t)render_target.texture.id;

    // UV0 and UV1 are flip vertically because raylib target texture are upside down
    ImGui::Image(texId, ImVec2((float)lastTexW, (float)lastTexH), ImVec2(0,1), ImVec2(1,0));

    ImGui::End();*/
    GuiManager::GuiFrame(registry);
    
    rlImGuiEnd();
    EndDrawing();

    return true;
}

bool EditorRenderingSystem::Close()
{
    // Clean the texture + raylib
    UnloadRenderTexture(render_target);
    CloseWindow();

    return true;
}




