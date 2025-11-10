#include <Koji/Koji.h>

#include "Koji/ECS/Systems/RenderSystem.h"


using namespace Koji::ECS;

int main(){

    Koji::Scene myScene("My Koji Scene");
    
    myScene.AddSystem<RenderSystem>();
    
    auto e = myScene.GetWorld()->CreateEntity("My entity");
    
    kSphere sphere {3.0f, 15, 10};
    myScene.GetWorld()->AddComponent(e, kSphere::TypeId, &sphere);
    
    
    
    return Koji::Application::Run(&myScene);
}
