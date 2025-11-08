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
        std::vector<std::unique_ptr<System>> systems;
        bool isRunning = false;

    public:
        Entity CreateEntity(const std::string& name);
        void AddComponent(Entity e, size_t componentId, void* data);
        std::vector<Archetype*> Query(const std::vector<size_t>& componentIds);
        
        template<typename... ComponentTypes>
        void ForComponents(std::function<void(ComponentTypes&...)> callback);
    
        template<typename... ComponentTypes>
        void ForEntitiesWithComponents(std::function<void(Entity, ComponentTypes&...)> callback);
        
        template<typename T, typename... Args>
        T* AddSystem(Args&&... args);
        
        template<typename T>
        T* GetSystem();
        
        // World loop
        void Start();
        void Update();
        void Stop();
    };
}
