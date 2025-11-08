#include <Koji/Koji.h>


using namespace Koji::ECS;

int main(){

    Koji::Scene myScene {
        "Koji Engine - Development",
        World(),
        { new TimeSystem() }
    };
    
    auto e = myScene.world.CreateEntity("My Sphere");

    kSphere sphere {3.0f, 15, 10};
    myScene.world.AddComponent(e, kSphere::TypeId, &sphere);
    
    
    
    return Koji::Application::Run(&myScene);
}
