#include <cstring>
#include <Koji/Core/Application.h>
#include "entt/entt.hpp"
#include "Koji/ECS/Components/Transform.h"
#include <iostream>
using Koji::Application;


Application::Application(const char* name, const uint16_t window_width, const uint16_t window_height) {
    this->name = new char[strlen(name) + 1]();
    strcpy(this->name, name);

    this->window_width = window_width;
    this->window_height = window_height;
}

bool Application::Run() const {
    // Init raylib
    InitWindow(window_width, window_height, name);

    // ECS
    entt::registry registry;


    

    // Loop
    while (!WindowShouldClose()) {
        // Update ECS / input if neededd
        

        // TODO in renderer
        // BeginDrawing();
        // ClearBackground(RAYWHITE);
        
        // BeginMode3D();
        // EndMode3D();

        // EndDrawing();
        
    }

    // Shutdown
    CloseWindow();
    return true;
}

Application::~Application() {
    free(this->name);
}

