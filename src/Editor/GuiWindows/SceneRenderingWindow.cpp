#include <cstdint>
#include <Koji/Editor/GuiWindows.hpp>
#include <Koji/Editor/Editor.hpp>

#include "imgui_internal.h"
#include "raylib.h"
#include "Koji/Engine/Scene.h"

using namespace Koji::Engine;

void SceneViewWindow::CheckRenderer()
{
    if (renderer || !active)
        return;

    for (System* sys : Editor::scene->systems)
        if (auto r_sys = dynamic_cast<Systems::EditorRenderingSystem*>(sys)) {
            renderer = r_sys;
            return;
        }
}



void SceneViewWindow::Draw()
{
    CheckRenderer();
    
    if (!ImGui::Begin("Scene - 3D View", &active)) {
        ImGui::End();
        return;
    }

    // Allow the ImGui window user to request a particular viewport size (or use content region size)
    ImVec2 avail = ImGui::GetContentRegionAvail();

    // If the content region changed, resize the render texture to match (or scale as you want)
    int wantW = (int)avail.x;
    int wantH = (int)avail.y;

    if (wantW <= 0) wantW = 64;
    if (wantH <= 0) wantH = 64;

    if (wantW != lastTexW || wantH != lastTexH) {
        UnloadRenderTexture(renderer->render_target);
        renderer->render_target = LoadRenderTexture(wantW, wantH);
        lastTexW = wantW;
        lastTexH = wantH;
    }

    // Convert from Raylib/OpenGl texture id -> ImGui texture id
    auto texId = (ImTextureID)(intptr_t)renderer->render_target.texture.id;

    // UV0 and UV1 are flip vertically because raylib target texture are upside down
    ImGui::Image(texId, ImVec2((float)lastTexW, (float)lastTexH), ImVec2(0,1), ImVec2(1,0));

    ImGui::End();
}
