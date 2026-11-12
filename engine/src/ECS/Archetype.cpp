#include <Koji/ECS/Archetype.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <utility>

namespace Koji::ECS
{
    Archetype::Archetype(ArchetypeId id, ChunkLayout layout)
        : Archetype(id, std::shared_ptr<const ChunkLayout>(std::make_shared<ChunkLayout>(std::move(layout))))
    {
    }

    Archetype::Archetype(ArchetypeId id, std::shared_ptr<const ChunkLayout> layout)
        : id_(id), layout_(std::move(layout))
    {
    }

    Archetype Archetype::Clone() const
    {
        Archetype copy(id_, layout_);
        copy.chunks_.reserve(chunks_.size());
        for (const Chunk& chunk : chunks_)
            copy.chunks_.push_back(chunk.Clone());
        copy.entityCount_ = entityCount_;
        copy.firstFree_ = firstFree_;
        return copy;
    }

    EntityRecord Archetype::Allocate(EntityId entity)
    {
        while (firstFree_ < chunks_.size() && chunks_[firstFree_].IsFull())
            ++firstFree_;
        if (firstFree_ == chunks_.size())
            chunks_.emplace_back(*layout_);

        const std::uint32_t row = chunks_[firstFree_].Push(entity);
        ++entityCount_;

        EntityRecord record;
        record.archetype = id_;
        record.chunk = static_cast<std::uint32_t>(firstFree_);
        record.row = row;
        return record;
    }

    EntityId Archetype::Remove(const EntityRecord& record)
    {
        if (!IsValid(record))
            return kNullEntity;

        const EntityId moved = chunks_[record.chunk].SwapRemove(record.row);
        --entityCount_;
        // This chunk now have space
        firstFree_ = std::min<std::size_t>(firstFree_, record.chunk);
        return moved;
    }

    MoveResult Archetype::MoveRow(const EntityRecord& from, Archetype& to)
    {
        if (!IsValid(from))
            return MoveResult{};                       // fail : newRecord.archetype == kInvalidArchetype
        if (&to == this)
            return MoveResult{from, kNullEntity};      // nothing to do

        Chunk& sourceChunk = chunks_[from.chunk];
        const EntityId entity = sourceChunk.Entities()[from.row];

        const EntityRecord destination = to.Allocate(entity);
        Chunk& destinationChunk = to.chunks_[destination.chunk];

        for (const ChunkColumn& column : to.layout_->Columns())
        {
            if (column.size == 0 || !layout_->Has(column.id))
                continue;   // tag, or component which did not existed in the source

            std::memcpy(destinationChunk.ComponentAt(column.id, destination.row),
                        sourceChunk.ComponentAt(column.id, from.row),
                        column.size);
        }

        const EntityId movedInSource = Remove(from);
        return MoveResult{destination, movedInSource};
    }

    Chunk& Archetype::GetChunk(std::uint32_t index)
    {
        assert(index < chunks_.size());
        return chunks_[index];
    }

    const Chunk& Archetype::GetChunk(std::uint32_t index) const
    {
        assert(index < chunks_.size());
        return chunks_[index];
    }

    void Archetype::TrimEmptyChunks()
    {
        while (!chunks_.empty() && chunks_.back().IsEmpty())
            chunks_.pop_back();
        firstFree_ = std::min(firstFree_, chunks_.size());
    }

    bool Archetype::IsValid(const EntityRecord& record) const
    {
        return record.archetype == id_
            && record.chunk < chunks_.size()
            && record.row < chunks_[record.chunk].Count();
    }
}