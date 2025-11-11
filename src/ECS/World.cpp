#include "Koji/ECS/World.h"

#include <algorithm>
#include <stdexcept>

#include "Koji/ECS/System.h"
#include "Koji/ECS/Components/Transform.h"

using namespace Koji::ECS;

Entity World::CreateEntity(const std::string& name) {
    Entity e = entities.Create();

    MetaData meta(name, 0, true);
    Transform t;

    std::vector<size_t> ids = { kComponent<MetaData>::TypeId, kComponent<Transform>::TypeId };

    Archetype* target = nullptr;
    for (auto* a : archetypes)
        if (a->Matches(ids)) { target = a; break; }

    if (!target) {
        target = new Archetype();
        target->componentIds = ids;
        archetypes.push_back(target);
    }

    std::unordered_map<size_t, void*> data {
            { kComponent<MetaData>::TypeId, &meta },
            { kComponent<Transform>::TypeId, &t }
    };

    target->AddEntity(data);
    return e;
}

void World::AddComponent(Entity e, size_t componentId, void* data) {
    Archetype* current = nullptr;
    for (auto* a : archetypes) {
        if (a->ContainsEntity(e)) {
            current = a;
            break;
        }
    }
    if (!current) return;

    if (std::find(current->componentIds.begin(), current->componentIds.end(), componentId)
        != current->componentIds.end()) return;

    std::vector<size_t> newIds = current->componentIds;
    newIds.push_back(componentId);

    Archetype* target = nullptr;
    for (auto* a : archetypes)
        if (a->Matches(newIds)) { target = a; break; }

    if (!target) {
        target = new Archetype();
        target->componentIds = newIds;
        archetypes.push_back(target);
    }

    std::unordered_map<size_t, void*> copyData;
    for (auto id : current->componentIds) {
        copyData[id] = current->GetComponentData(e, id);
    }
    copyData[componentId] = data;

    target->AddEntity(copyData);
    current->RemoveEntity(e);
}

std::vector<Archetype*> World::Query(const std::vector<size_t>& componentIds) {
    std::vector<Archetype*> result;
    for (auto* a : archetypes)
        if (a->Matches(componentIds))
            result.push_back(a);
    return result;
}

