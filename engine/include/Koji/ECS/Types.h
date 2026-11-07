#pragma once
#include <bitset>
#include <cstdint>

namespace Koji::ECS
{
    using ComponentId = std::uint32_t;
    inline constexpr ComponentId kInvalidComponent = 0xFFFFFFFFu;

    inline constexpr std::size_t kMaxComponents = 256;
    using ComponentMask = std::bitset<kMaxComponents>;

    using ArchetypeId = std::uint32_t;
    inline constexpr ArchetypeId kInvalidArchetype = 0xFFFFFFFFu;

}