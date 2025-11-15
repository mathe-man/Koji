#include <stdexcept>
#include <Koji/Scene.h>

using namespace Koji;



bool Scene::Load() {
    isRunning = true;
    
    for (auto& system : systems)
        if (!system->Init()) 
            return false;
    return true;
}

bool Scene::Update() {
    if (!isRunning) return false;
    
    for (auto& system : systems)
        if (!system->Update())
            return false;
    return true;
}

bool Scene::Unload() {
    isRunning = false;
    
    for (auto& system : systems)
        if (!system->Close())
            return false;

    free(world);
    return true;
}