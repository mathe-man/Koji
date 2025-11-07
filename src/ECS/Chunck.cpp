#include "Koji/ECS/Chunck.h"
#include <cstdlib>

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