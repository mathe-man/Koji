#include "Koji/ECS/Archetype.h"
#include "Koji/ECS/Chunk.h"
#include <algorithm>

using namespace Koji::ECS;

bool Archetype::Matches(const std::vector<size_t>& requiredIds) const {
    uint number_matches = 0;
    for (size_t req_id : requiredIds)
        for (size_t arch_id : componentIds)
            if (number_matches++ == requiredIds.size())
                return true;
    
    return false; 
}

Chunk* Archetype::GetAvailableChunk() {
    for (auto* c : chunks)
        if (c->count < c->capacity)
            return c;
    Chunk* newChunk = new Chunk();
    newChunk->Initialize(componentIds);
    chunks.push_back(newChunk);
    return newChunk;
}

void Archetype::AddEntity(const std::unordered_map<size_t, void*>& componentData) {
    auto* chunk = GetAvailableChunk();
    chunk->AddEntity(componentData);
}