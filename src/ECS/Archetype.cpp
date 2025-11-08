#include "Koji/ECS/Archetype.h"
#include "Koji/ECS/Chunck.h"
#include <algorithm>

using namespace Koji::ECS;

bool Archetype::Matches(const std::vector<size_t>& requiredIds) const {
    for (auto requiredId : requiredIds) {
        if (std::find(componentIds.begin(), componentIds.end(), requiredId) == componentIds.end())
            return false;  // A component isn't present
    }
    return true;  // Every required component are present 
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