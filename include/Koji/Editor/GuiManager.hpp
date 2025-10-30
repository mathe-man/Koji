#pragma once
#include <Koji/Editor/Systems.hpp>

#include <imgui.h>

#include "Editor.hpp"



namespace Koji::Editor
{
    struct MenuNode
    {
        std::string name;
        bool active = false;
        std::function<void()> callback = nullptr;
        std::vector<MenuNode*> childrens = std::vector<MenuNode*>();
    }; 
    
    class GuiManager
    {
    public:
        static bool Init(entt::registry& registry);
        static bool GuiFrame(entt::registry& registry);


        
        
        static MenuNode root;
    private:
        static void CreateMainDockspace();
        static void MenuBar();
        static void RenderMenuNode(MenuNode node);
    };
} // namespace Koji::Editor
