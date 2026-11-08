#include <Koji/ECS/EntityManager.h>

namespace Koji::ECS
{
    EntityId EntityManager::Create()
    {
        std::uint32_t index = 0;
        if (!freeList_.empty())
        {
            index = freeList_.back();
            freeList_.pop_back();
        }
        else
        {
            if (slots_.size() >= EntityId::kNullIndex)  // Maximum entity
                return kNullEntity;
            index = static_cast<std::uint32_t>(slots_.size());
            slots_.emplace_back();
        }

        Slot& slot = slots_[index];
        slot.alive = true;
        slot.record = EntityRecord{};
        ++aliveCount_;
        return EntityId{index, slot.generation};
    }

    bool EntityManager::Destroy(EntityId entity)
    {
        if (!IsAlive(entity))
            return false;

        Slot& slot = slots_[entity.index];
        slot.alive = false;
        ++slot.generation;              // Make every previous EntityId of this slot invalid
        slot.record = EntityRecord{};   // Clear
        freeList_.push_back(entity.index);
        --aliveCount_;
        return true;
    }

    bool EntityManager::IsAlive(EntityId entity) const
    {
        if (entity.index >= slots_.size())
            return false;
        const Slot& slot = slots_[entity.index];
        return slot.alive && slot.generation == entity.generation;
    }

    std::optional<EntityRecord> EntityManager::Locate(EntityId entity) const
    {
        if (!IsAlive(entity))
            return std::nullopt;
        return slots_[entity.index].record;
    }

    bool EntityManager::SetLocation(EntityId entity, const EntityRecord& record)
    {
        if (!IsAlive(entity))
            return false;
        slots_[entity.index].record = record;
        return true;
    }
}