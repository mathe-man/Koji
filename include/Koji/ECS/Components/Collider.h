#pragma once
#include <reactphysics3d/reactphysics3d.h>

#include "glm/vec3.hpp"

namespace Koji::ECS
{
    struct Collider
    {
        reactphysics3d::CollisionShape* shape = nullptr;
        reactphysics3d::Collider* collider = nullptr;

        glm::vec3 halfExtents;
    };
}
