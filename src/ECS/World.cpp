#include "Koji/ECS/World.h"

#include <algorithm>
#include <stdexcept>

#include "Koji/ECS/System.h"
#include "Koji/ECS/Components/Transform.h"

using namespace Koji::ECS;

Entity World::CreateEntity(const std::string& name) {
    Entity e = entities.Create();

    MetaData meta(name, 0, true);
    kTransform t({0,0,0}, {0,0,0}, {1,1,1});

    std::vector<size_t> ids = { kComponent<MetaData>::TypeId, kComponent<kTransform>::TypeId };

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
            { kComponent<kTransform>::TypeId, &t }
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


template<typename... ComponentTypes>
void World::ForComponents(std::function<void(ComponentTypes&...)> callback) {
    
    std::vector<size_t> componentIds = { kComponent<ComponentTypes>::TypeId... };
    auto matchingArchetypes = Query(componentIds);
    
    // Check if we have a result
    if (matchingArchetypes.empty())
        return;
    
    
    for (auto* archetype : matchingArchetypes) {
        for (auto* chunk : archetype->chunks) {
            // Check if the chunk is not empty
            if (chunk->count == 0) continue;
            
            // Retrieve arrays of components
            std::tuple<ComponentTypes*...> componentArrays = {
                static_cast<ComponentTypes*>(chunk->GetComponentArray(kComponent<ComponentTypes>::TypeId))...
            };
            
            // Check that all arrays are good
            bool allArraysValid = (... && (std::get<ComponentTypes*>(componentArrays) != nullptr));
            if (!allArraysValid) continue;
            
            // Iterate over the arrays
            for (size_t i = 0; i < chunk->count; i++)
                callback((*std::get<ComponentTypes*>(componentArrays))[i]...);
        }
    }
}

template<typename... ComponentTypes>
void World::ForEntitiesWithComponents(std::function<void(Entity, ComponentTypes&...)> callback) {
    
    std::vector<size_t> componentIds = { kComponent<ComponentTypes>::TypeId... };
    auto matchingArchetypes = Query(componentIds);
    
    // Check if we have a result
    if (matchingArchetypes.empty())
        return;
    
    
    for (auto* archetype : matchingArchetypes) {
        for (auto* chunk : archetype->chunks)
        {
            // Check if the chunk is not empty
            if (chunk->count == 0) continue;
            
            // Retrieve arrays of components
            std::tuple<ComponentTypes*...> componentArrays = {
                static_cast<ComponentTypes*>(chunk->GetComponentArray(kComponent<ComponentTypes>::TypeId))...
            };
            
            // Check that all arrays are good
            bool allArraysValid = (... && (std::get<ComponentTypes*>(componentArrays) != nullptr));
            if (!allArraysValid) continue;
            
            // Iterate over the arrays
            for (size_t i = 0; i < chunk->count; i++) {
                Entity entity = chunk->entities[i];
                callback(entity, (*std::get<ComponentTypes*>(componentArrays))[i]...);
            }
        }
    }
}


