#include <Koji/Koji.h>

#include "Koji/ECS/Components/Collider.h"
#include "Koji/ECS/Components/PhysicsBody.h"
#include "Koji/ECS/Components/Transform.h"
#include "Koji/ECS/Systems/PhysicsSystem.h"
#include "Koji/ECS/Systems/RenderSystem.h"
#include "Koji/ECS/Systems/Time.h"


using namespace Koji;
using namespace Koji::ECS;


int main(){

    Koji::Scene myScene("My Koji Scene");

    myScene.events->subscribe<SystemInitiedEvent<RenderSystem>>(
        [](SystemInitiedEvent<RenderSystem>) {
            std::cout << "RenderSystem intitialisation has been detected via an event" << std::endl;
        });

    myScene.events->subscribe<SystemClosedEvent<RenderSystem>>(
        [](SystemClosedEvent<RenderSystem>) {
            std::cout << "RenderSystem intitialisation has been detected via an event" << std::endl;
        });
    
    auto* sys_Render = myScene.AddSystem<RenderSystem>();
    auto* sys_Physics = myScene.AddSystem<PhysicsSystem>(); // TODO Fix and test the physic system
    auto* sys_Time = myScene.AddSystem<TimeSystem>();
    
    auto sphere = myScene.world->create();
    myScene.world->emplace<Transform>(sphere);
    
    
    return Koji::Application::Run(&myScene);
}
