#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32

#include "Koji/Core/Application.h"
#include "Koji/Core/Time.h"
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
    ScopeTimer t = ScopeTimer([](double seconds) {
        std::cout << "Scope timer ended after " << seconds << "s\n";
    });

    Core::Application::Init (
        Core::ApplicationData {
            "Koji Game Engine",
            WINDOW_WIDTH, WINDOW_HEIGHT,
            entt::registry()
        }
    );

    DelayTimer timer(2500, [] {
        std::cout << "Delay timer finished\n";
    });
    RepeatedDelayTimer r_timer(1000, [](int i) {
        std::cout << "Repeated delay iterated " << i << " times\n";
    });

    timer.Start();
    r_timer.Start();

    return Core::Application::Run();
}
