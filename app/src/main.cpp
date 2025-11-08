#include <Koji/Koji.h>




int main(){

    Koji::Scene myScene {
        "Koji Engine - Development",
        Koji::World(),
        { new Koji::ECS::TimeSystem() }
    };
    
    auto e = myScene.world.CreateEntity("My Sphere");

    Koji::ECS::kSphere sphere {3.0f, 15, 10};
    myScene.world.AddComponent(e, Koji::ECS::kSphere::TypeId, &sphere);
    
    
    
    return Koji::Application::Run(&myScene);
}
