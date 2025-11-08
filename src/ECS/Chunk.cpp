#include "Koji/ECS/Chunk.h"
#include <cstdlib>

using namespace Koji::ECS;

void Chunk::Initialize(const std::vector<size_t>& componentIds) {
    size_t totalSize = 0;
    for (auto id : componentIds)
        totalSize += ComponentRegistry::Get(id).size;

    capacity = CHUNK_SIZE / totalSize;

    for (auto id : componentIds) {
        const auto& info = ComponentRegistry::Get(id);
        columns[id].data = std::aligned_alloc(info.alignment, info.size * capacity);
        columns[id].componentSize = info.size;
    }
}

void* Chunk::GetComponentArray(size_t componentId) {
    return columns[componentId].data;
}

void Chunk::AddEntity(const std::unordered_map<size_t, void*>& componentData) {
    for (auto& [id, src] : componentData) {
        auto& col = columns[id];
        std::memcpy((char*)col.data + col.componentSize * count, src, col.componentSize);
    }
    count++;
}

bool Chunk::ContainsEntity(Entity e) const
{
    return std::find(entities.begin(), entities.end(), e) != entities.end();
}

void* Chunk::GetComponentData(Entity e, size_t componentId)
{
    auto it = std::find(entities.begin(), entities.end(), e);
    if (it == entities.end()) return nullptr;
    size_t index = std::distance(entities.begin(), it);

    auto& col = columns[componentId];
    return static_cast<void*>(static_cast<char*>(col.data) + index * col.componentSize);
}

void Chunk::RemoveEntity(Entity e)
{
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
