#pragma once
#include <queue>
#include "Entity.h"

namespace Koji::ECS
{
    class EntityManager {
        std::queue<uint32_t> freeIds;
        uint32_t nextId = 0;

    public:
        Entity Create();
        void Destroy(Entity e);
    };
}