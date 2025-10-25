#include <Koji/Engine/Scene.h>
#include <Koji/Engine/Application.hpp>
#include "Koji/Engine/Systems.hpp"


using namespace Koji::Engine;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;
    scene->systems.insert(scene->systems.begin(), new RenderingSystem);
    
    // ECS
    entt::registry reg = std::move(scene->registry);

    for (System* sys : scene->systems)
        if (!sys->Init(*scene, reg))
            return false;
    
    
    
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

