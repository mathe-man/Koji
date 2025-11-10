#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include "Archetype.h"
#include "EntityManager.h"
#include "Components/MetaData.h"

namespace Koji::ECS
{
    class System;

    class World
    {
        std::vector<Archetype*> archetypes;
        EntityManager entities;

    public:
        Entity CreateEntity(const std::string& name);
        void AddComponent(Entity e, size_t componentId, void* data);
        void RemoveComponent(Entity e, size_t componentId);
        std::vector<Archetype*> Query(const std::vector<size_t>& componentIds);
        
        template<typename... ComponentTypes>
        void ForComponents(std::function<void(ComponentTypes&...)> callback) {
            std::vector<size_t> componentIds = { kComponent<ComponentTypes>::TypeId... };
            auto matchingArchetypes = Query(componentIds);
    
            for (auto* archetype : matchingArchetypes) {
                for (auto* chunk : archetype->chunks) {
                    if (chunk->count == 0) continue;
            
                    std::tuple<ComponentTypes*...> componentArrays = {
                        static_cast<ComponentTypes*>(chunk->GetComponentArray(kComponent<ComponentTypes>::TypeId))...
                    };
            
                    for (size_t i = 0; i < chunk->count; i++) {
                        callback(std::get<ComponentTypes*>(componentArrays)[i]...);
                    }
                }
            }
        }
    
        template<typename... ComponentTypes>
        void ForEntitiesWithComponents(std::function<void(Entity, ComponentTypes&...)> callback) {
            std::vector<size_t> componentIds = { kComponent<ComponentTypes>::TypeId... };
            auto matchingArchetypes = Query(componentIds);
    
            for (auto* archetype : matchingArchetypes) {
                for (auto* chunk : archetype->chunks) {
                    if (chunk->count == 0) continue;
            
                    std::tuple<ComponentTypes*...> componentArrays = {
                        static_cast<ComponentTypes*>(chunk->GetComponentArray(kComponent<ComponentTypes>::TypeId))...
                    };
            
                    for (size_t i = 0; i < chunk->count; i++) {
                        Entity entity = chunk->entities[i];
                        callback(entity, (*std::get<ComponentTypes*>(componentArrays))[i]...);
                    }
                }
            }
        }
    
        size_t GetEntityCount() const;
        size_t GetArchetypeCount() const;
    };
}
