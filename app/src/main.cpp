#include <Koji/Koji.h>


using namespace Koji::ECS;

int main(){

    Koji::Scene myScene("My Koji Scene");
    
    auto e = myScene.GetWorld()->CreateEntity("My entity");

    kSphere sphere {3.0f, 15, 10};
    myScene.GetWorld()->AddComponent(e, kSphere::TypeId, &sphere);
    
    
    
    return Koji::Application::Run(&myScene);
}
