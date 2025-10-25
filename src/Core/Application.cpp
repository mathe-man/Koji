#include <cstring>
#include <iostream>
#include <raylib.h>
#include <Koji/Core/Application.h>
#include <Koji/ECS//Systems/Systems.hpp>
#include <Koji/ECS//Components/Components.hpp>

using namespace Koji::Core;
using namespace Koji::Systems;

Scene* Application::scene {}; // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;
    scene->systems.insert(scene->systems.begin(), new RenderingSystem);
    
    // ECS
    entt::registry reg = std::move(scene->registry);

    for (System* sys : scene->systems)
        if (!sys->Init(*scene, reg))
            return false;
    
    
    
    std::cout << GetScreenHeight() << std::endl;
    // Loop
    while (true) {

        for (System* sys : scene->systems)
            if (!sys->BeginFrame(reg))
                return false;
        
        for (System* sys : scene->systems)
            if (!sys->Frame(reg))
                return false;

        for (System* sys : scene->systems)
            if (!sys->EndFrame(reg))
                return false;
    }

    // Shutdown
    for (System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}

