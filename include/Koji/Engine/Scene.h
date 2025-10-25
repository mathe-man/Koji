#pragma once
#include <cstdint>
#include <vector>
#include <entt/entt.hpp>

namespace Koji::Engine {

    // Forward declaration
    class System;
    
    struct Scene
    {
        const char* name;
        uint16_t window_width;
        uint16_t window_height;
        entt::registry registry;
        std::vector<System*> systems;
    };
    
} // Koji::Engine