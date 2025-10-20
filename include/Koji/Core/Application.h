#pragma once
#include <cstdint>

namespace Koji {

    class Application {
    public:
        Application(const char* name, uint16_t window_width, uint16_t window_height);
        bool Run() const;
        ~Application();
    private:
        char *name;
        uint16_t window_width;
        uint16_t window_height;
    };

} // Koji