#include <Koji/Editor/GuiManager.hpp>
#include <Koji/Editor/GuiWindows.hpp>


using namespace Koji::Editor;

bool GuiManager::Init(entt::registry& registry)
{
    // Change ImGui default styles
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Title bar colors === Set the same color for different states of the window
    style.Colors[ImGuiCol_TitleBg]           = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // unfocused
    style.Colors[ImGuiCol_TitleBgActive]     = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // focused
    style.Colors[ImGuiCol_TitleBgCollapsed]  = ImVec4(0.15f, 0.15f, 0.15f, 1.0f); // collapsed
    
    return true;
}

std::vector<GuiWindow*> GuiManager::windows = {};

bool GuiManager::GuiFrame(entt::registry& registry)
{
    CreateMainDockspace();

    for (GuiWindow* win : windows)
        if (win->active)
            win->Draw();
    
    return true;
}


MenuNode GuiManager::root { "Editor Main Menu", false, nullptr,
    {
        new MenuNode { "Window", false, nullptr,
            {
                new MenuNode {"Gui Demo", false, []{windows.push_back(new GuiDemoWindow());}},
                new MenuNode {"Scene view", false, []{windows.push_back(new SceneViewWindow());}},
                new MenuNode {"Scene entites", false, []{windows.push_back(new SceneEntitiesTree());}}
            }
        }
    }
};

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
    // Menu bar height
    
    ImGui::Begin("Koji Editor", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    // Create dockspace
    ImGuiID dockspace_id = ImGui::GetID("Editor dockspace");
    ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);


    MenuBar(); // Create the menu bar
    
    ImGui::End();
}


void GuiManager::MenuBar()
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{10, 14});
    
    if (ImGui::BeginMainMenuBar()) {
        
        ImGui::PopStyleVar(); // Pop style var
        
        for (MenuNode* node : root.childrens)
            if (!node->callback && ImGui::BeginMenu(node->name.c_str())) {
                for (MenuNode* child_node : node->childrens)
                    RenderMenuNode(*child_node);

                ImGui::EndMenu();
            }
        
        ImGui::EndMainMenuBar();
    }
    else
        ImGui::PopStyleVar(); // Pop style var if BeginMainMenuBar returned false
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
