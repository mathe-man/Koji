#include "Koji/Core/Application.h"
using namespace Koji::Core;

// define the screen size to 1920*1080 (HD) or 1280*720
#ifdef HD_WINDOW_SIZE
    constexpr uint16_t WINDOW_WIDTH = 1920;
    constexpr uint16_t WINDOW_HEIGHT = 1080;
#else
    constexpr uint16_t WINDOW_WIDTH = 1280;
    constexpr uint16_t WINDOW_HEIGHT = 720;
#endif

int main(){

    Application::Init (
        ApplicationData {
            "Koji Engine - Development", WINDOW_WIDTH, WINDOW_HEIGHT, entt::registry()
    });
    
    return Application::Run();
}
