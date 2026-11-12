#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

#include <Koji/ECS/Chunk.h>
#include <Koji/ECS/ChunkLayout.h>
#include <Koji/ECS/Entity.h>
#include <Koji/ECS/EntityManager.h>
#include <Koji/ECS/Types.h>

namespace Koji::ECS
{
    // Result of a MoveRow() : two entity moved, the World have to update them
    struct MoveResult
    {
        EntityRecord newRecord;                 // Where is the moved entity
                                                // newRecord.archetype == kInvalidArchetype if it failled
        EntityId movedInSource = kNullEntity;    // Entity who take the place of the moved one
                                                 // kNullEntity if none
    };

    // Exact combination of components. Own the ChunkLayout and the list of chunks who store entities of this Archetype
    // The returned EntityRecord are up-to-date until the next Remove or MoveRow affecting the same row
    class Archetype
    {
    public:
        Archetype(ArchetypeId id, ChunkLayout layout);
        Archetype(Archetype&&) noexcept = default;
        Archetype& operator=(Archetype&&) noexcept = default;
        Archetype(const Archetype&) = delete;
        Archetype& operator=(const Archetype&) = delete;

        // Independent clone: same entities, datas. Both archetypes share the layout (which never changed after his creation)
        Archetype Clone() const;

        ArchetypeId Id() const { return id_; }
        const ChunkLayout& Layout() const { return *layout_; }
        const ComponentMask& Signature() const { return layout_->Signature(); }
        bool Has(ComponentId id) const { return layout_->Has(id); }

        // Store the entity in the first chunk with space (or create a new chunk if needed)
        // His components are 'zero'.
        // Return his location
        EntityRecord Allocate(EntityId entity);

        // Supprime la ligne désignée (SwapRemove). Renvoie l'entité qui a comblé le trou,
        // ou kNullEntity si personne n'a bougé. Un emplacement invalide (autre archetype, chunk ou
        // ligne hors limites) ne fait rien et renvoie aussi kNullEntity.
        // Delete the chosen row. Return the entity which take his in this row, or kNullEntity if nobody moved.
        // A wrong location (bad archetype, chunk or out of range row) do nothing and return kNullEntity.
        EntityId Remove(const EntityRecord& record);


        // Move the entity in 'to' (other archetype) : Copy components already existing in this archetype,
        // new components in 'to' are 'zero' and datas of removed components is lost.
        // Both archetypes must come from the same ComponentRegistry.
        // 'to' == *this : nothing happen and return 'from'. Invalid location => fail (see MoveResult)
        MoveResult MoveRow(const EntityRecord& from, Archetype& to);

        std::size_t ChunkCount() const { return chunks_.size(); }
        std::size_t EntityCount() const { return entityCount_; }
        Chunk& GetChunk(std::uint32_t index);
        const Chunk& GetChunk(std::uint32_t index) const;
        std::span<Chunk> Chunks() { return chunks_; }
        std::span<const Chunk> Chunks() const { return chunks_; }

        // Free empty chunks at the end of the vector.
        // Empty chunks are never freed automatically when empty, avoid reserving and freeing memory a lot of time
        void TrimEmptyChunks();

    private:
        Archetype(ArchetypeId id, std::shared_ptr<const ChunkLayout> layout);

        bool IsValid(const EntityRecord& record) const;

        ArchetypeId id_;
        std::shared_ptr<const ChunkLayout> layout_;   // chunks keep a ptr
        std::vector<Chunk> chunks_;
        std::size_t entityCount_ = 0;
        std::size_t firstFree_ = 0;   // chunks before this index are full
    };
}