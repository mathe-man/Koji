#include <cstring>
#include <iostream>
#include <raylib.h>
#include <Koji/Core/Application.h>
#include <Koji/ECS//Systems/Systems.hpp>
#include <Koji/ECS//Components/Components.hpp>

using namespace Koji::Core;
using namespace Koji::Systems;

ApplicationData Application::data {};
bool Application::as_initiated = false;

bool Application::Init(ApplicationData d)
{
    data = std::move(d);
    
    as_initiated = true;
    return true;
}


bool Application::Run(const bool exit_at_end) {
    if (!as_initiated)
        throw std::runtime_error("The application first need to be initiated with ApplicationData");

    // ECS
    entt::registry reg = std::move(data.registry);

    for (System* sys : data.systems)
        if (!sys->Init(data, reg))
            return false;
    
    
    
    std::cout << GetScreenHeight() << std::endl;
    // Loop
    while (true) {

        for (System* sys : data.systems)
            if (!sys->BeginFrame(reg))
                return false;
        
        for (System* sys : data.systems)
            if (!sys->Frame(reg))
                return false;

        for (System* sys : data.systems)
            if (!sys->EndFrame(reg))
                return false;
    }

    // Shutdown
    for (System*  sys : data.systems)
        if (!sys->Close())
            return false;

    
    return true;
}

void Application::Exit()
{
}

