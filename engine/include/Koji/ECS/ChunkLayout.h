#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include <Koji/ECS/ComponentRegistry.h>
#include <Koji/ECS/Entity.h>
#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    inline constexpr std::size_t kDefaultChunkBytes = 16 * 1024;
    inline constexpr std::size_t kChunkMinAlignment = 64;   // On cache row

    // One chunk's column
    struct ChunkColumn
    {
        ComponentId id = kInvalidComponent;
        std::uint32_t size = 0;    // 1 element size ; 0 for a tag => no memory
        std::uint32_t align = 1;
        std::size_t offset = 0;    // start of this colomn in the chunk
    };

    // Memory plan of a chunk, calculated one time by the archetype and shared between all his chunk
    //
    //   [ EntityId x N ][ column A x N ][ column B x N ] ...   (N = capacity)
    //
    // The most strongly aligned columns are placed first to limit padding
    class ChunkLayout
    {
    public:
        // chunkBytes : chunk size (ex. kDefaultChunkBytes).
        // components : components of the archetype
        // Return std::nullopt if :
        // - a ComponentId appear multiple
        // - the number of component is >= kMaxComponents
        // - a single entity doesn't fit in the chunk
        static std::optional<ChunkLayout> Create(std::size_t chunkBytes,
                                                 const std::vector<ComponentInfo>& components);

        std::uint32_t Capacity() const { return capacity_; }      // Entity per chunk
        std::size_t ChunkBytes() const { return chunkBytes_; }
        std::size_t Alignment() const { return alignment_; }      // The chunk's alignment
        const ComponentMask& Signature() const { return signature_; }
        const std::vector<ChunkColumn>& Columns() const { return columns_; }

        bool Has(ComponentId id) const { return FindColumn(id) != nullptr; }
        // Return nullptr if the component isn't here
        const ChunkColumn* FindColumn(ComponentId id) const;

        // Entities IDs column is always the first one
        static constexpr std::size_t EntityColumnOffset() { return 0; }

    private:
        ChunkLayout() = default;

        // Place columns for 'count' entity
        // Return the total size needed
        std::size_t ComputeOffsets(std::size_t count);

        std::size_t chunkBytes_ = 0;
        std::size_t alignment_ = kChunkMinAlignment;
        std::uint32_t capacity_ = 0;
        ComponentMask signature_;
        std::vector<ChunkColumn> columns_;
        std::array<std::int16_t, kMaxComponents> columnOf_{};   // ComponentId -> index in columns_, -1 if absent
    };
}