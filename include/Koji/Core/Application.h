#pragma once
#include <cstdint>

#include "entt/entity/registry.hpp"

namespace Koji::Core {

    struct ApplicationData
    {
        const char* name;
        uint16_t window_width;
        uint16_t window_height;
        entt::registry start_registry;
    };

    class Application {
    public:
        static bool Init(ApplicationData d);
        static bool Run(bool exit_at_end = true);
        static void Exit();

        static ApplicationData data;
    };

} // Koji