#include <Koji/Koji.h>


using namespace Koji::Engine;



constexpr uint16_t WINDOW_WIDTH = 1280;
constexpr uint16_t WINDOW_HEIGHT = 720;


int main(){

    Scene myScene {
        "Koji Engine - Development",
        WINDOW_WIDTH, WINDOW_HEIGHT,
        EntityRegistry(),
        std::vector<System*>() = { new TimeSystem() }
    };
    
    auto entity = myScene.entities.create();
    auto* transform = myScene.entities.AddComponent<Components::kTransform>(entity);
    auto* sphere = myScene.entities.AddComponent<Components::kSphere>(entity);
    sphere->rings = 20;
    sphere->slices = 20;
    
    return Application::Run(&myScene);
}
