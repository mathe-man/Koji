#pragma once
#include "bgfx/bgfx.h"
#include "glfw/glfw3.h"

namespace Koji {
    class Renderer {
    public:
        bool Init(GLFWwindow* window, bgfx::RendererType::Enum render_type = bgfx::RendererType::Count);
        void Shutdown();
        void BeginFrame();
        void EndFrame();

        uint32_t GetViewId() const { return m_viewId; }

    private:
        void SetupView();
        uint32_t m_width = 0;
        uint32_t m_height = 0;
        bgfx::ViewId m_viewId = 0;
    };
} // Koji