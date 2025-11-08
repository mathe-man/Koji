#include <Koji/ECS/System.h>
#include <Koji/Scene.h>
#include <Koji/Application.h>

using namespace Koji;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;

    for (ECS::System* sys : scene->systems)
        if (!sys->Init())
            return false;
    
    
    
    // Loop
    while (true) {

        
        for (ECS::System* sys : scene->systems)
            if (!sys->Update())
                return false;
    }

    // Shutdown
    for (ECS::System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}

