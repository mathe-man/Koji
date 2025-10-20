#include <iostream>

#define GLFW_EXPOSE_NATIVE_WIN32

#include "Koji/Core/Application.h"
#include "Koji/Core/Time.h"
using namespace Koji;


// define the screen size to 1920*1080 (HD) or 1280*720
#ifdef HD_WINDOW_SIZE
    constexpr uint32_t WINDOW_WIDTH = 1920;
    constexpr uint32_t WINDOW_HEIGHT = 1080;
#else
    constexpr uint32_t WINDOW_WIDTH = 1280;
    constexpr uint32_t WINDOW_HEIGHT = 720;
#endif

int main(){
    
    auto app = Application("Hello World", WINDOW_WIDTH, WINDOW_HEIGHT);

    return app.Run();
}
