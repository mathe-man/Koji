#include <Koji/ECS/ChunkLayout.h>

#include <algorithm>
#include <limits>

namespace Koji::ECS
{
    std::optional<ChunkLayout> ChunkLayout::Create(std::size_t chunkBytes,
                                                   const std::vector<ComponentInfo>& components)
    {
        ChunkLayout layout;
        layout.chunkBytes_ = chunkBytes;
        // Fill with -1
        layout.columnOf_.fill(-1);

        std::size_t bytesPerEntity = sizeof(EntityId);
        std::size_t alignment = kChunkMinAlignment;

        layout.columns_.reserve(components.size());
        for (const ComponentInfo& info : components)
        {
            if (info.id >= kMaxComponents)
                return std::nullopt;
            // True if this id as already been set
            if (layout.signature_.test(info.id))
                return std::nullopt;   // double
            // Set
            layout.signature_.set(info.id);

            layout.columns_.push_back(ChunkColumn{info.id, info.size, info.align, 0});
            bytesPerEntity += info.size;
            alignment = std::max<std::size_t>(alignment, info.align);
        }
        layout.alignment_ = alignment;

        // Most-aligned columns first: less padding. Ties keep the given order
        std::stable_sort(layout.columns_.begin(), layout.columns_.end(),
                         [](const ChunkColumn& a, const ChunkColumn& b) { return a.align > b.align; });
        for (std::size_t i = 0; i < layout.columns_.size(); ++i)
            layout.columnOf_[layout.columns_[i].id] = static_cast<std::int16_t>(i);

        // Capacity : the biggest N for which every columns can be stored in the chunk, paddings included
        std::size_t count = chunkBytes / bytesPerEntity;
        while (count > 0 && layout.ComputeOffsets(count) > chunkBytes)
            --count;
        if (count == 0 || count > std::numeric_limits<std::uint32_t>::max())
            return std::nullopt;

        layout.capacity_ = static_cast<std::uint32_t>(count);
        return layout;
    }

    const ChunkColumn* ChunkLayout::FindColumn(ComponentId id) const
    {
        if (id >= kMaxComponents)
            return nullptr;
        const std::int16_t index = columnOf_[id];
        return index < 0 ? nullptr : &columns_[static_cast<std::size_t>(index)];
    }

    std::size_t ChunkLayout::ComputeOffsets(std::size_t count)
    {
        std::size_t offset = count * sizeof(EntityId);   // EntityId first column
        for (ChunkColumn& column : columns_)
        {
            if (column.size == 0)
            {
                column.offset = 0;   // tag => no memory
                continue;
            }
            offset = (offset + column.align - 1) / column.align * column.align;   // Rounded to the next alignment multiple
            column.offset = offset;
            offset += count * column.size;  // Adding the number of bytes in the column (column.size is the size of 1 of is element)
        }
        return offset;
    }
}