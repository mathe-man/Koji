#pragma once
#include <bgfx/bgfx.h>

namespace Koji {

    class Application {
    public:
        Application(const char* name, uint16_t window_width, uint16_t window_height, bgfx::RendererType::Enum render_type = bgfx::RendererType::Count);
        bool Run() const;
        ~Application();
    private:
        char *name;
        uint16_t window_width;
        uint16_t window_height;
        bgfx::RendererType::Enum render_type;
    };

} // Koji