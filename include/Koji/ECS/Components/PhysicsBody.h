#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace Koji::ECS
{
    struct PhysicsBody {
        reactphysics3d::RigidBody* body = nullptr;

        float mass = 1.0f;
        bool isStatuc = false;
    };   
}