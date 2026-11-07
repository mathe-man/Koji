#pragma once
#include <cstdint>

namespace Koji::ECS
{
    // Entity id = index + génération
    // When an entity is destroyed is index can be reused but with an incremented generation
    struct EntityId
    {
        static constexpr std::uint32_t kNullIndex = 0xFFFFFFFFu;

        std::uint32_t index = kNullIndex;
        std::uint32_t generation = 0;

        constexpr bool IsNull() const { return index == kNullIndex; }
        friend constexpr bool operator==(const EntityId&, const EntityId&) = default;
    };

    inline constexpr EntityId kNullEntity{};
}