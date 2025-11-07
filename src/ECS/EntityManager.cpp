#include "Koji/ECS/EntityManager.h"

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

void EntityManager::Destroy(Entity e) {
    freeIds.push(e.id);
}