#pragma once
#include "Time.h"
#include "entt/entity/registry.hpp"
#include "Koji/Application.h"
#include "Koji/Scene.h"
#include "reactphysics3d/reactphysics3d.h"

namespace Koji::ECS {
    
    class PhysicsSystem : public System {
    public:
        reactphysics3d::PhysicsCommon physicsCommon;
        reactphysics3d::PhysicsWorld* world;

        PhysicsSystem() {
            reactphysics3d::PhysicsWorld::WorldSettings settings;
            world = physicsCommon.createPhysicsWorld(settings);
        }

        bool Update() override {
            SyncTransformsToPhysics(*Application::scene->world);
            world->update(std::max(Application::scene->GetSystem<TimeSystem>()->GetDeltaTime(), 0.000001f));
            SyncTransformsFromPhysics(*Application::scene->world);

            return true;
        }

    private:
        void SyncTransformsToPhysics(entt::registry& ecs);
        void SyncTransformsFromPhysics(entt::registry& ecs);
    };
}