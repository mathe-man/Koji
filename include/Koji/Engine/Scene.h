#pragma once
#include <cstdint>
#include <vector>
#include <entt/entt.hpp>

#include "EntityRegistry.hpp"

namespace Koji::Engine {

    // Forward declaration
    class System;
    
    struct Scene
    {
        const char* name;
        uint16_t window_width;
        uint16_t window_height;
        EntityRegistry entities;
        std::vector<System*> systems;
    };
    
} // Koji::Engine