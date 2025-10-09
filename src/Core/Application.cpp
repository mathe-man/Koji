#include <Koji/Core/Application.h>
using Koji::Application;

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <glfw/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

#include <iostream>


Application::Application(const char* name, const uint16_t window_width, const uint16_t window_height, bgfx::RendererType::Enum render_type) {
    this->name = new char[strlen(name) + 1]();
    strcpy(this->name, name);

    this->window_width = window_width;
    this->window_height = window_height;
    this->render_type = render_type;
}

bool Application::Run() const {
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return false;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // bgfx gère le rendu
    GLFWwindow* window = glfwCreateWindow(this->window_width, this->window_height, "GLFW + BGFX", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return false;
    }

    // setuo bgfx init settings
    bgfx::Init init;
    init.type               = this->render_type;
    init.vendorId           = BGFX_PCI_ID_NONE;
    init.resolution.width   = this->window_width;
    init.resolution.height  = this->window_height;
    init.resolution.reset   = BGFX_RESET_VSYNC;
    init.resolution.format  = bgfx::TextureFormat::BGRA8;

    bgfx::PlatformData pd{};
    pd.nwh          = glfwGetWin32Window(window);
    pd.ndt          = nullptr;
    pd.context      = nullptr;
    pd.backBuffer   = nullptr;
    pd.backBufferDS = nullptr;
    init.platformData = pd; // Giving the platform data this way instead of using bgfx::PlatformData avoid the segfault


    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize bgfx!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    // Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff);
        bgfx::setViewRect(0, 0, 0, this->window_width, this->window_height);
        bgfx::touch(0); // ensure view 0 is cleared
        bgfx::frame();
    }

    // Shutdown
    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}

Application::~Application() {
    free(this->name);
}

