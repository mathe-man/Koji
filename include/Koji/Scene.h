#pragma once
#include <cstdint>
#include <vector>

#include "ECS/World.h"

namespace Koji::Engine {

    // Forward declaration
    class System;
    
    struct Scene
    {
        std::string name;
        World world;
        std::vector<System*> systems;
    };
    
} // Koji::Engine