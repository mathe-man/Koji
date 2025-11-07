#pragma once
#include <vector>
#include <unordered_map>
#include "Archetype.h"
#include "EntityManager.h"
#include "Components/MetaData.h"
#include "Components/Transform.h"

class World {
    std::vector<Archetype*> archetypes;
    EntityManager entities;

public:
    Entity CreateEntity(const std::string& name);
    void AddComponent(Entity e, size_t componentId, void* data);
    std::vector<Archetype*> Query(const std::vector<size_t>& componentIds);
};