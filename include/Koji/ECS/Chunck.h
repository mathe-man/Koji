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

        bool ContainsEntity(Entity e) const {
            return std::find(entities.begin(), entities.end(), e) != entities.end();
        }

        void* GetComponentData(Entity e, size_t componentId) {
            auto it = std::find(entities.begin(), entities.end(), e);
            if (it == entities.end()) return nullptr;
            size_t index = std::distance(entities.begin(), it);

            auto& col = columns[componentId];
            return static_cast<void*>(static_cast<char*>(col.data) + index * col.componentSize);
        }

        void RemoveEntity(Entity e) {
            auto it = std::find(entities.begin(), entities.end(), e);
            if (it == entities.end()) return;

            size_t index = std::distance(entities.begin(), it);
            size_t last = entities.size() - 1;

            // swap last entity into the removed slot
            entities[index] = entities[last];

            for (auto& [id, col] : columns) {
                char* dataPtr = static_cast<char*>(col.data);
                std::memcpy(dataPtr + index * col.componentSize,
                            dataPtr + last * col.componentSize,
                            col.componentSize);
            }

            entities.pop_back();
            count--;
        }
    };
}