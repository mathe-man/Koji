#include <Koji/Core/Application.h>

#include <Koji/Render/Renderer.h>
#include "Koji/ECS/Systems/RenderingSystem.h"
#include "entt.hpp"

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

    Renderer renderer;
    if (!renderer.Init(window)) return -1;

    entt::registry registry;
    RenderingSystem renderingSystem(&renderer);



    // Loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        renderingSystem.Render(registry);
    }

    // Shutdown
    renderer.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}

Application::~Application() {
    free(this->name);
}

