#pragma once
#include <cstdint>

struct Entity {
    uint32_t id;
    uint32_t version;
    bool operator==(const Entity& other) const { return id == other.id && version == other.version; }
};