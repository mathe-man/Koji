#pragma once
#include "imgui.h"
#include "Systems.hpp"

using namespace Koji::Editor;

namespace Koji::Editor
{
    class GuiWindow
    {
    public:
        virtual ~GuiWindow() = default;

        virtual void Draw() = 0;
    };


    class SceneViewWindow : public GuiWindow
    {
    public:
        void Draw() override;

    private:
        void CheckRenderer();
        Systems::EditorRenderingSystem* renderer = nullptr;

    
        int lastTexW = 0;
        int lastTexH = 0;
    };
} // namesapce Koji::Editor