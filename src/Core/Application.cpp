#include <cstring>
#include <raylib.h>
#include <Koji/Core/Application.h>
#include <Koji/ECS//Systems/Systems.hpp>

using Koji::Application;
using Koji::Systems::RenderingSystem;


Application::Application(const char* name, const uint16_t window_width, const uint16_t window_height) {
    this->name = new char[strlen(name) + 1]();
    strcpy(this->name, name);

    this->window_width = window_width;
    this->window_height = window_height;
}

bool Application::Run() const {
    // ECS
    entt::registry reg;

    auto rendering_system = RenderingSystem(window_width, window_height, name);
    
    

    // Loop
    while (!WindowShouldClose()) {
        // Update ECS / input if neededd
        
        rendering_system.BeginFrame(reg);
        
        rendering_system.Frame(reg);

        rendering_system.EndFrame(reg);
    }

    // Shutdown
    CloseWindow();
    return true;
}

Application::~Application() {
    free(this->name);
}

