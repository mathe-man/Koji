#pragma once
#include <string>
#include <vector>

#include "ECS/System.h"
#include "ECS/World.h"

class World;

struct Scene
{
    std::string name;
    World world;
    std::vector<System*> systems;
};
