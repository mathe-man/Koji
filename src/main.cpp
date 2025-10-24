#include "Koji/Core/Application.h"
#include "Koji/ECS/Systems/Systems.hpp"
#include "Koji/ECS/Components/Components.hpp"


using namespace Koji;

// define the screen size to 1920*1080 (HD) or 1280*720
#ifdef HD_WINDOW_SIZE
    constexpr uint16_t WINDOW_WIDTH = 1920;
    constexpr uint16_t WINDOW_HEIGHT = 1080;
#else
    constexpr uint16_t WINDOW_WIDTH = 1280;
    constexpr uint16_t WINDOW_HEIGHT = 720;
#endif

int main(){

    Core::Application::Init (
        Core::ApplicationData {
            "Koji Engine - Development",
            WINDOW_WIDTH, WINDOW_HEIGHT,
            entt::registry(),
            {new Koji::Systems::RenderingSystem()}
    });
    
    return Core::Application::Run();
}
