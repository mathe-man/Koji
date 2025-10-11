#include <Koji/Render/Renderer.h>
#include <iostream>
#include <glfw/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

using namespace Koji;

bool Renderer::Init(GLFWwindow *window, bgfx::RendererType::Enum render_type) {
    if (!window)
        return false;


    // Get the window size and set resolution settings
    int w, h;
    glfwGetWindowSize(window, &w, &h);
    m_width = static_cast<uint32_t>(w);
    m_height = static_cast<uint32_t>(h);

    // Get native window handle
    bgfx::PlatformData pd{};
    pd.nwh          = glfwGetWin32Window(window);
    pd.ndt          = nullptr;
    pd.context      = nullptr;
    pd.backBuffer   = nullptr;
    pd.backBufferDS = nullptr;

    // setup bgfx init settings
        // Misc
    bgfx::Init init;
    init.type               = render_type;
    init.vendorId           = BGFX_PCI_ID_NONE;
        // Resolution params
    init.resolution.width   = m_width;
    init.resolution.height  = m_height;
    init.resolution.reset   = BGFX_RESET_VSYNC;
    init.resolution.format  = bgfx::TextureFormat::BGRA8;
        // Add the platform data
    init.platformData = pd; // Giving the platform data this way instead of using bgfx::PlatformData avoid the segfault


    //
    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize bgfx!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }
    return true;
}

void Renderer::SetupView() {
    bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xffffff, 1.0f, 0);
    bgfx::setViewRect(m_viewId, 0, 0, m_width, m_height);
}

void Renderer::BeginFrame() {
    bgfx::touch(m_viewId);
}

void Renderer::EndFrame() {
    bgfx::frame();
}

void Koji::Renderer::Shutdown() {
    bgfx::shutdown();
}

