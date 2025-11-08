#include <Koji/ECS/System.h>
#include <Koji/Scene.h>
#include <Koji/Application.h>


using namespace Koji::Engine;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;

    for (System* sys : scene->systems)
        if (!sys->Init())
            return false;
    
    
    
    // Loop
    while (true) {

        
        for (System* sys : scene->systems)
            if (!sys->Update())
                return false;
    }

    // Shutdown
    for (System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}

