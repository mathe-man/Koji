#include <Koji/Koji.h>

#include "Koji/ECS/Systems/RenderSystem.h"
#include "Koji/ECS/Systems/Time.h"


using namespace Koji::ECS;


int main(){

    Koji::Scene myScene("My Koji Scene");
    
    myScene.AddSystem<RenderSystem>();
    myScene.AddSystem<TimeSystem>();
    
    
    
    return Koji::Application::Run(&myScene);
}
