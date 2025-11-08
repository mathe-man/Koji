#include <iostream>
#include <Koji/Koji.h>

#include "Koji/ECS/Systems/Time.h"


using namespace Koji::Engine;



int main(){

    Scene myScene {
        "Koji Engine - Development",
        World(),
        { new TimeSystem() }
    };
    
    auto e = myScene.world.CreateEntity("My Sphere");
    
    kSphere sphere {3.0f, 15, 10};
    myScene.world.AddComponent(e, kSphere::TypeId, &sphere);
    
    
    
    return Application::Run(&myScene);
}
