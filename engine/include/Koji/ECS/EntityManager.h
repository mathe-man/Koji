#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include <Koji/ECS/Entity.h>
#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    // This is where an entity live : which archetype, chunk, row
    // Updated by the World when an entity is moved or created
    struct EntityRecord
    {
        ArchetypeId archetype = kInvalidArchetype;
        std::uint32_t chunk = 0;
        std::uint32_t row = 0;
    };

    // Dispatch EntityId and keep track of where each entity is located
    class EntityManager
    {
    public:
        // New entity (record.archetype == kInvalidArchetype).
        // Return kNullEntity if all the slots are taken
        EntityId Create();

        // Return false if the entity is not alive (already destroyed, old generation, null...).
        // The slot is recycled with a +1 generation
        bool Destroy(EntityId entity);

        bool IsAlive(EntityId entity) const;

        // Return std::nullopt if the entity is not alive
        std::optional<EntityRecord> Locate(EntityId entity) const;

        // Return false if the entity isn't alive
        bool SetLocation(EntityId entity, const EntityRecord& record);

        std::size_t AliveCount() const { return aliveCount_; }

    private:
        struct Slot
        {
            std::uint32_t generation = 0;   // go back to 0 after 2^32 recycling of the same slot
            bool alive = false;
            EntityRecord record;
        };

        std::vector<Slot> slots_;                   // Indexed by EntityId::index
        std::vector<std::uint32_t> freeList_;       // free slot, last freed is reused first
        std::size_t aliveCount_ = 0;
    };
}