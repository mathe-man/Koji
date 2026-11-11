#include <Koji/ECS/Chunk.h>

#include <cstring>
#include <new>
#include <utility>

namespace Koji::ECS
{
    Chunk::Chunk(const ChunkLayout& layout)
        : layout_(&layout)
    {
        data_ = static_cast<std::byte*>(
            ::operator new(layout.ChunkBytes(), std::align_val_t{layout.Alignment()}));
        std::memset(data_, 0, layout.ChunkBytes());
    }

    Chunk::~Chunk()
    {
        if (data_)
            ::operator delete(data_, std::align_val_t{layout_->Alignment()});
    }

    Chunk::Chunk(Chunk&& other) noexcept
        : layout_(other.layout_), data_(other.data_), count_(other.count_)
    {
        other.data_ = nullptr;
        other.count_ = 0;
    }

    Chunk& Chunk::operator=(Chunk&& other) noexcept
    {
        if (this != &other)
        {
            if (data_)
                ::operator delete(data_, std::align_val_t{layout_->Alignment()});
            layout_ = other.layout_;
            data_ = other.data_;
            count_ = other.count_;
            other.data_ = nullptr;
            other.count_ = 0;
        }
        return *this;
    }

    Chunk Chunk::Clone() const
    {
        Chunk copy(*layout_);
        if (data_)
        {
            std::memcpy(copy.data_, data_, layout_->ChunkBytes());
            copy.count_ = count_;
        }
        return copy;
    }

    std::uint32_t Chunk::Push(EntityId entity)
    {
        if (!data_ || count_ >= layout_->Capacity())
            return kInvalidRow;

        const std::uint32_t row = count_++;
        EntityPtr()[row] = entity;
        for (const ChunkColumn& column : layout_->Columns())
        {
            if (column.size > 0)
                std::memset(data_ + column.offset + std::size_t{row} * column.size, 0, column.size);
        }
        return row;
    }

    EntityId Chunk::SwapRemove(std::uint32_t row)
    {
        if (!data_ || row >= count_)
            return kNullEntity;

        const std::uint32_t last = count_ - 1;
        EntityId moved = kNullEntity;
        if (row != last)
        {
            EntityId* entities = EntityPtr();
            entities[row] = entities[last];
            moved = entities[row];

            for (const ChunkColumn& column : layout_->Columns())
            {
                if (column.size > 0)
                    std::memcpy(data_ + column.offset + std::size_t{row} * column.size,
                                data_ + column.offset + std::size_t{last} * column.size,
                                column.size);
            }
        }
        --count_;
        return moved;
    }

    std::span<EntityId> Chunk::Entities()
    {
        return data_ ? std::span<EntityId>(EntityPtr(), count_) : std::span<EntityId>();
    }

    std::span<const EntityId> Chunk::Entities() const
    {
        return data_ ? std::span<const EntityId>(EntityPtr(), count_) : std::span<const EntityId>();
    }

    std::byte* Chunk::ColumnPtr(ComponentId id) const
    {
        const ChunkColumn* column = layout_->FindColumn(id);
        if (!data_ || !column || column->size == 0)
            return nullptr;
        return data_ + column->offset;
    }

    std::byte* Chunk::ElementPtr(ComponentId id, std::uint32_t row) const
    {
        const ChunkColumn* column = layout_->FindColumn(id);
        if (!data_ || !column || column->size == 0 || row >= count_)
            return nullptr;
        return data_ + column->offset + std::size_t{row} * column->size;
    }
}