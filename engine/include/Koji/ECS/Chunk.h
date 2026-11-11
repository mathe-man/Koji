#pragma once
#include <cstddef>
#include <cstdint>
#include <span>

#include <Koji/ECS/ChunkLayout.h>
#include <Koji/ECS/Entity.h>
#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    // A memory chunk of fixed size, storing entities sharing the same archetype,
    // one column per component type
    //
    // Components are trivially copyable
    // The ChunkLayout should live longer than the Chunk (It keep a non-owning pointer)
    // A moved chunk (std::move) is empty and unusable, it can be destroyed
    class Chunk
    {
    public:
        static constexpr std::uint32_t kInvalidRow = 0xFFFFFFFFu;

        explicit Chunk(const ChunkLayout& layout);
        ~Chunk();
        Chunk(Chunk&& other) noexcept;
        Chunk& operator=(Chunk&& other) noexcept;
        Chunk(const Chunk&) = delete;
        Chunk& operator=(const Chunk&) = delete;

        // Same entities and data
        Chunk Clone() const;

        std::uint32_t Count() const { return count_; }
        std::uint32_t Capacity() const { return layout_->Capacity(); }
        bool IsEmpty() const { return count_ == 0; }
        bool IsFull() const { return count_ >= layout_->Capacity(); }
        const ChunkLayout& Layout() const { return *layout_; }

        // Add a row at the end, the entity components are null.
        // Return the row number or kInvalidRow if the chunk is full
        std::uint32_t Push(EntityId entity);

        // Delete the 'row' and move the last row at this position (=> No hole and offset)
        // Return the newly placed  in 'row' (his position changed so World have to update it)
        // Return kNullEntity if nothing changed: 'row' was the last row of the chunk or was out of range
        EntityId SwapRemove(std::uint32_t row);

        void Clear() { count_ = 0; }

        std::span<EntityId> Entities();
        std::span<const EntityId> Entities() const;

        // Start of component column (Count() valid elements) ;
        // nullptr if it's a tag or absent
        void* Column(ComponentId id) { return ColumnPtr(id); }
        const void* Column(ComponentId id) const { return ColumnPtr(id); }

        // Address of the component at 'row' ; nullptr if absent, tag, or out of range
        void* ComponentAt(ComponentId id, std::uint32_t row) { return ElementPtr(id, row); }
        const void* ComponentAt(ComponentId id, std::uint32_t row) const { return ElementPtr(id, row); }


        // Return a typed column
        // Return an empty span if:
        //  - The component is missing
        //  - T don't match the size and alignment of the component
        //  - The component is a tag
        template <typename T>
        std::span<T> Components(ComponentId id)
        {
            return std::span<T>(TypedPtr<T>(id), TypedPtr<T>(id) ? count_ : 0);
        }
        template <typename T>
        std::span<const T> Components(ComponentId id) const
        {
            return std::span<const T>(TypedPtr<T>(id), TypedPtr<T>(id) ? count_ : 0);
        }

    private:
        std::byte* ColumnPtr(ComponentId id) const;
        std::byte* ElementPtr(ComponentId id, std::uint32_t row) const;

        template <typename T>
        T* TypedPtr(ComponentId id) const
        {
            const ChunkColumn* column = layout_->FindColumn(id);
            if (!column || column->size != sizeof(T) || column->align < alignof(T))
                return nullptr;
            return reinterpret_cast<T*>(ColumnPtr(id));
        }

        EntityId* EntityPtr() const { return reinterpret_cast<EntityId*>(data_); }

        const ChunkLayout* layout_;
        std::byte* data_ = nullptr;
        std::uint32_t count_ = 0;
    };
}