#include <Koji/ECS/Systems/PhysicsSystem.h>

#include "Koji/ECS/Components/PhysicsBody.h"
#include "Koji/ECS/Components/Transform.h"

#include <iostream>

using namespace Koji::ECS;

void PhysicsSystem::SyncTransformsToPhysics(entt::registry& ecs) {
    auto view = ecs.view<PhysicsBody, Transform>();
    
    view.each(
        [&](PhysicsBody& pb, Transform& tr) {

            if (!pb.body) return;

            // Convert glm → reactphysics3d structures
            reactphysics3d::Vector3 pos(tr.position.x, tr.position.y, tr.position.z);
            reactphysics3d::Quaternion rot(tr.rotation.x, tr.rotation.y,
                                           tr.rotation.z, tr.rotation.w);

            reactphysics3d::Transform t(pos, rot);

            pb.body->setTransform(t);
        }
    );
}


void PhysicsSystem::SyncTransformsFromPhysics(entt::registry& ecs) {
    auto view = ecs.view<PhysicsBody, Transform>();
    
    view.each(
        [&](PhysicsBody& pb, Transform& tr) {

            if (!pb.body) return;

            reactphysics3d::Transform t = pb.body->getTransform();
            reactphysics3d::Vector3 pos = t.getPosition();
            reactphysics3d::Quaternion rot = t.getOrientation();

            tr.position = glm::vec3(pos.x, pos.y, pos.z);
            std::cout << "x:" << tr.position.x << " y:" << tr.position.y << " z:" << tr.position. z << std::endl;
            tr.rotation = glm::quat(rot.w, rot.x, rot.y, rot.z);
        }
    );
}
