#pragma once
#include <Koji/Editor/Systems.hpp>

#include <imgui.h>

#include "Editor.hpp"



namespace Koji::Editor
{
    
    
    class GuiManager
    {
    public:
        static bool Init(entt::registry& registry);
        static bool GuiFrame(entt::registry& registry);

        static void AddMenuItem(const std::string& path, std::function<void()> func);

    private:
        static void CreateMainDockspace();
        static void MenuBar();
    };
} // namespace Koji::Editor
