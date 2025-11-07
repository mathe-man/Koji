#include <Koji/Scene.h>
#include <Koji/Application.h>
#include <Koji/Systems.hpp>


using namespace Koji::Engine;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;
    scene->systems.insert(scene->systems.begin(), new RaylibRenderingSystem);
    
    

    for (System* sys : scene->systems)
        if (!sys->Init(*scene))
            return false;
    
    
    
    // Loop
    while (true) {

        for (System* sys : scene->systems)
            if (!sys->BeginFrame())
                return false;
        
        for (System* sys : scene->systems)
            if (!sys->Frame())
                return false;

        for (System* sys : scene->systems)
            if (!sys->EndFrame())
                return false;
    }

    // Shutdown
    for (System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}

