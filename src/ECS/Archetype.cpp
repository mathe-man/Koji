#include "Koji/ECS/Archetype.h"
#include "Koji/ECS/Chunck.h"
#include <algorithm>


bool Archetype::Matches(const std::vector<size_t>& ids) const {
    if (ids.size() != componentIds.size()) return false;
    for (auto id : ids)
        if (std::find(componentIds.begin(), componentIds.end(), id) == componentIds.end())
            return false;
    return true;
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