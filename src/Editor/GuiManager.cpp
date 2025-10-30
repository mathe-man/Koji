#include <Koji/Editor/GuiManager.hpp>


using namespace Koji::Editor;

bool GuiManager::Init(entt::registry& registry)
{
    return true;
}

bool GuiManager::GuiFrame(entt::registry& registry)
{
    CreateMainDockspace();
    
    return true;
}


void GuiManager::CreateMainDockspace()
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
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    // Remove border effects
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

    ImGui::Begin("Koji Editor", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Create dockspace
    ImGuiID dockspace_id = ImGui::GetID("Editor dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);


    MenuBar(); // Create the menu bar
    
    ImGui::End();
}


MenuNode GuiManager::root { "Editor Main Menu", false, nullptr,
{
    new MenuNode { "File", false, nullptr,
{
            new MenuNode { "Open"  },
            new MenuNode { "Edit"  },
            new MenuNode { "Close" }
            }
    },
    new MenuNode { "Debugging", false, nullptr,
        {
            new MenuNode { "Time profiler" },
            new MenuNode { "Memory" }
        }
    },
    new MenuNode { "Misc", false, nullptr,
        {
            new MenuNode { "Add node", false,
                []
                {
                    GuiManager::root.childrens.push_back(new MenuNode { "New node"});
                }}
        }
    }
    
}
};


void GuiManager::MenuBar()
{
            if (ImGui::BeginMenuBar()) {
        for (MenuNode* node : root.childrens)
            if (!node->callback && ImGui::BeginMenu(node->name.c_str())) {
                for (MenuNode* child_node : node->childrens)
                    RenderMenuNode(*child_node);

                ImGui::EndMenu();
            }
        
        ImGui::EndMenuBar();
    }
}


void GuiManager::RenderMenuNode(MenuNode node)
{
    if (!node.callback && ImGui::BeginMenu(node.name.c_str())) // Create menu if there is no callback
    {
        for (MenuNode* child_node : node.childrens)
            RenderMenuNode(*child_node);

        ImGui::EndMenu();
    }

    else if (   node.callback &&
                ImGui::MenuItem(node.name.c_str(), nullptr, &node.active))
        node.callback(); // Call the  callback if the item is clicked

}
