#pragma once
#include "entt/entity/registry.hpp"
#include "reactphysics3d/reactphysics3d.h"

namespace Koji::ECS {
    
    class PhysicsSystem {
    public:
        reactphysics3d::PhysicsCommon physicsCommon;
        reactphysics3d::PhysicsWorld* world;

        PhysicsSystem() {
            reactphysics3d::PhysicsWorld::WorldSettings settings;
            world = physicsCommon.createPhysicsWorld(settings);
        }

        void Update(float dt, entt::registry& ecs) {
            SyncTransformsToPhysics(ecs);
            world->update(dt);
            SyncTransformsFromPhysics(ecs);
        }

    private:
        void SyncTransformsToPhysics(entt::registry& ecs);
        void SyncTransformsFromPhysics(entt::registry& ecs);
    };
}