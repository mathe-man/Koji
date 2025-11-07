#pragma once
#include <vector>
#include <unordered_map>

#include "Chunck.h"

struct Entity;
struct Chunk;

struct Archetype {
    std::vector<size_t> componentIds;
    std::vector<Chunk*> chunks;

    bool Matches(const std::vector<size_t>& ids) const;
    Chunk* GetAvailableChunk();
    void AddEntity(const std::unordered_map<size_t, void*>& componentData);
    
    bool ContainsEntity(Entity e) const {
        for (auto* chunk : chunks)
            if (chunk->ContainsEntity(e))
                return true;
        return false;
    }
    void* GetComponentData(Entity e, size_t componentId) {
        for (auto* chunk : chunks) {
            if (chunk->ContainsEntity(e))
                return chunk->GetComponentData(e, componentId);
        }
        return nullptr;
    }
    void RemoveEntity(Entity e) {
        for (auto* chunk : chunks) {
            if (chunk->ContainsEntity(e)) {
                chunk->RemoveEntity(e);
                break;
            }
        }
    }

};
