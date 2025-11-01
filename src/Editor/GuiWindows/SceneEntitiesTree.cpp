#include <Koji/Editor/GuiWindows.hpp>
#include "Koji/Editor/Editor.hpp"

using namespace Koji::Engine;

void SceneEntitiesTree::Draw()
{
    if (!ImGui::Begin("Entities Tree", &active)) {
        ImGui::End();
        return;
    }

    auto view = Editor::scene->registry.view<Components::kMetaData>();

    
    int selected = -1; 
    int i = 0;
    
    for (auto entity : view) {
        auto& meta = view.get<Components::kMetaData>(entity);
        bool isSelected = (selected == i);

        // show entity name
        ImGui::Selectable(meta.name, isSelected);

        // left click
        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            selected = i;

        // right click
        if (ImGui::BeginPopupContextItem(meta.name)) {
            //TODO add rename or delete popup
            ImGui::EndPopup();
        }

        i++;
    }


    ImGui::End();
}
