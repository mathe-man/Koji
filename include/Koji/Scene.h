#pragma once
#include <string>
#include <vector>

#include "ECS/System.h"
#include "ECS/World.h"

namespace Koji
{
    struct Scene
    {
        std::string name;
        ECS::World world;
        std::vector<ECS::System*> systems;
    };
}
