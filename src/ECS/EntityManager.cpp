#include "Koji/ECS/EntityManager.h"
#include "Koji/ECS/Entity.h"

using namespace Koji::ECS;

Entity EntityManager::Create() {
    uint32_t id;
    if (!freeIds.empty()) {
        id = freeIds.front();
        freeIds.pop();
    } else {
        id = nextId++;
    }
    return { id, 1 };
}

void EntityManager::Destroy(ECS::Entity e) {
    freeIds.push(e.id);
}