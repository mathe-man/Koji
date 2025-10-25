#include <Koji/Engine/Scene.h>
#include <Koji/Engine/Application.hpp>
#include "Koji/Engine/Systems.hpp"


using namespace Koji::Engine;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;
    scene->systems.insert(scene->systems.begin(), new RaylibRenderingSystem);
    
    

    for (System* sys : scene->systems)
        if (!sys->Init(*scene, scene->registry))
            return false;
    
    
    
    // Loop
    while (true) {

        for (System* sys : scene->systems)
            if (!sys->BeginFrame(scene->registry))
                return false;
        
        for (System* sys : scene->systems)
            if (!sys->Frame(scene->registry))
                return false;

        for (System* sys : scene->systems)
            if (!sys->EndFrame(scene->registry))
                return false;
    }

    // Shutdown
    for (System*  sys : scene->systems)
        if (!sys->Close())
            return false;

    
    return true;
}

