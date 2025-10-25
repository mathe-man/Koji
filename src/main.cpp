#include <Koji/Editor.h>
#include <Koji/Engine.h>


using namespace Koji::Engine;
using namespace Koji::Editor;

// define the screen size to 1920*1080 (HD) or 1280*720
#ifdef HD_WINDOW_SIZE
    constexpr uint16_t WINDOW_WIDTH = 1920;
    constexpr uint16_t WINDOW_HEIGHT = 1080;
#else
    constexpr uint16_t WINDOW_WIDTH = 1280;
    constexpr uint16_t WINDOW_HEIGHT = 720;
#endif

int main(){

    Scene myScene {
        "Koji Engine - Development",
        WINDOW_WIDTH, WINDOW_HEIGHT,
        entt::registry(),
        std::vector<System*>()
    };
    
    return Editor::Run(&myScene);
}
