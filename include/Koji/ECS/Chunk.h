#pragma once
#include <algorithm>
#include <vector>
#include <cstdint>
#include <unordered_map>
#include <cstring>
#include "Component.h"
#include "Entity.h"

namespace Koji::ECS
{
    constexpr size_t CHUNK_SIZE = 16 * 1024; // 16 KB

    struct Chunk {
        struct Column {
            void* data = nullptr;
            size_t componentSize = 0;
        };

        std::unordered_map<size_t, Column> columns;
        std::vector<Entity> entities;   // <--- ajoute ça
        size_t capacity = 0;
        size_t count = 0;

        void Initialize(const std::vector<size_t>& componentIds);
        void* GetComponentArray(size_t componentId);
        
        void AddEntity(const std::unordered_map<size_t, void*>& componentData);
        bool ContainsEntity(Entity e) const;
        void RemoveEntity(Entity e);

        void* GetComponentData(Entity e, size_t componentId);
        template<typename T>
        T* GetComponent(Entity e) {
            void* data = GetComponentData(e, kComponent<T>::TypeId);
            return static_cast<T*>(data);
        }

        ~Chunk() {
            for (auto& [id, col] : columns)
                std::free(col.data);
        }
    };
}