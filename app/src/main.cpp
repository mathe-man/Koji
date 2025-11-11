#include <Koji/Koji.h>

#include "Koji/ECS/Systems/RenderSystem.h"


using namespace Koji::ECS;

void CreatePyramid(World& world);

int main(){

    Koji::Scene myScene("My Koji Scene");
    auto* world = myScene.GetWorld();
    
    myScene.AddSystem<RenderSystem>();
    
    
    
    return Koji::Application::Run(&myScene);
}
