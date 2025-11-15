#include <Koji/Koji.h>

#include "Koji/ECS/Systems/RenderSystem.h"


using namespace Koji::ECS;


int main(){

    Koji::Scene myScene("My Koji Scene");
    
    myScene.AddSystem<RenderSystem>();
    
    
    
    return Koji::Application::Run(&myScene);
}
