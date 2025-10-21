#include <cstring>
#include <raylib.h>
#include <Koji/Core/Application.h>
#include <Koji/ECS//Systems/Systems.hpp>

using namespace Koji::Core;
using Koji::Systems::RenderingSystem;

ApplicationData Application::data {};
bool Application::as_initiated = false;

bool Application::Init(ApplicationData d)
{
    data = std::move(d);
    as_initiated = true;
    return true;
}


bool Application::Run(bool exit) {
    if (!as_initiated)
        throw std::runtime_error("The application first need to be initiated with ApplicationData");

    // ECS
    entt::registry reg = std::move(data.start_registry);

    auto rendering_system = RenderingSystem(std::move(data));
    
    

    // Loop
    while (!WindowShouldClose()) {
        // Update ECS / input if neededd
        
        rendering_system.BeginFrame(reg);
        
        rendering_system.Frame(reg);

        rendering_system.EndFrame(reg);
    }

    // Shutdown
    CloseWindow();

    if (exit)
        Exit();
    
    return true;
}

void Application::Exit()
{
}

