#include <Koji/ECS/System.h>
#include <Koji/Scene.h>
#include <Koji/Application.h>

using namespace Koji;

Scene* Application::scene {};  // Static member need to be declared

bool Application::Run(Scene* s) {
    // Set the static member
    scene = s;
    if (!scene) return false;

    if (!scene->Load())
        return false;
    
    // Loop
    while (true) {
        if (!scene->Update())
            break;
    }

    // Shutdown
    if (!scene->Unload())
        return false;
    
    return true;
}

