#include <glfw3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>


// define the screen size to 1920*1080 (HD) or 1280*720
#ifdef HD_WINDOW_SIZE
    constexpr uint32_t WINDOW_WIDTH = 1920;
    constexpr uint32_t WINDOW_HEIGHT = 1080;
#else
    constexpr uint32_t WINDOW_WIDTH = 1280;
    constexpr uint32_t WINDOW_HEIGHT = 720;
#endif

int main() {
    // Init GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!\n";
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // bgfx gère le rendu
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLFW + BGFX", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create window!\n";
        glfwTerminate();
        return -1;
    }

    // setuo bgfx init settings
    bgfx::Init init;
    init.type = bgfx::RendererType::Vulkan;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = WINDOW_WIDTH;
    init.resolution.height = WINDOW_HEIGHT;
    init.resolution.reset = BGFX_RESET_VSYNC;
    init.resolution.format = bgfx::TextureFormat::BGRA8;

    bgfx::PlatformData pd{};
    pd.nwh = glfwGetWin32Window(window);
    pd.ndt = nullptr;
    pd.context = nullptr;
    pd.backBuffer = nullptr;
    pd.backBufferDS = nullptr;
    init.platformData = pd; // Giving the platform data this way instead of using bgfx::PlatformData avoid the segfault


    if (!bgfx::init(init)) {
        std::cerr << "Failed to initialize bgfx!\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }


    // Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff);
        bgfx::setViewRect(0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        bgfx::touch(0); // ensure view 0 is cleared
        bgfx::frame();
    }

    // Shutdown
    bgfx::shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
