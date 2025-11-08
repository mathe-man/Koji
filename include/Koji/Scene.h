#pragma once
#include <cstdint>
#include <vector>

#include  "ECS/System.h"
#include "ECS/World.h"


struct Scene
{
    std::string name;
    World world;
    std::vector<System*> systems;
};