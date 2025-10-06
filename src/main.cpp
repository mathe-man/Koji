#include <glfw3.h>
#include <bgfx/bgfx.h>
#include <iostream>

int main() {
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // bgfx gère le rendu
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW + BGFX", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    // Init bgfx
    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // auto-select renderer
    init.resolution.width = 800;
    init.resolution.height = 600;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    // Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff);
        bgfx::setViewRect(0, 0, 0, 800, 600);
        bgfx::touch(0); // ensure view 0 is cleared
        bgfx::frame();
    }

    // Shutdown
    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
