#include <doctest/doctest.h>

#include <cstdint>
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>

#include <Koji/ECS/Archetype.h>

using namespace Koji::ECS;

namespace
{
    struct Position { float x, y, z; };
    struct Velocity { float x, y, z; };
    struct Health { int value; };
    struct Flag {};

    struct Env
    {
        ComponentRegistry reg;
        ComponentId pos = reg.RegisterType<Position>();
        ComponentId vel = reg.RegisterType<Velocity>();
        ComponentId health = reg.RegisterType<Health>();
        ComponentId flag = reg.RegisterType<Flag>();

        Archetype Make(ArchetypeId id, std::size_t chunkBytes, std::initializer_list<ComponentId> ids) const
        {
            std::vector<ComponentInfo> infos;
            for (ComponentId component : ids)
                infos.push_back(*reg.Info(component));
            return Archetype(id, ChunkLayout::Create(chunkBytes, infos).value());
        }
    };

    EntityId E(std::uint32_t index) { return EntityId{index, 0}; }

    constexpr std::size_t kSmallChunk = 100;   // 5 entities with a position component per chunk
}


TEST_CASE("A new archetype is empty")
{
    Env env;
    const Archetype a = env.Make(7, kDefaultChunkBytes, {env.pos, env.vel});

    CHECK(a.Id() == 7u);    // Given Id
    CHECK(a.EntityCount() == 0);
    CHECK(a.ChunkCount() == 0);
    CHECK(a.Has(env.pos));
    CHECK(a.Has(env.vel));
    CHECK_FALSE(a.Has(env.health));
    CHECK(a.Signature().count() == 2);  // Archetype with two components
}

TEST_CASE("Allocate store the entity and return is location")
{
    Env env;
    Archetype a = env.Make(3, kDefaultChunkBytes, {env.pos});

    const EntityRecord r0 = a.Allocate(E(10));
    const EntityRecord r1 = a.Allocate(E(11));

    CHECK(r0.archetype == 3u);
    CHECK(r0.chunk == 0u);
    CHECK(r0.row == 0u);
    CHECK(r1.row == 1u);
    CHECK(a.EntityCount() == 2);
    CHECK(a.ChunkCount() == 1);
    CHECK(a.GetChunk(0).Entities()[0] == E(10));
    CHECK(a.GetChunk(0).Entities()[1] == E(11));
}

TEST_CASE("Allocate create a new chunk when the first one is full")
{
    Env env;
    Archetype a = env.Make(0, kSmallChunk, {env.pos});
    REQUIRE(a.Layout().Capacity() == 5u);

    EntityRecord last;
    for (std::uint32_t i = 0; i < 6; ++i)
        last = a.Allocate(E(i));

    CHECK(a.ChunkCount() == 2);
    CHECK(a.EntityCount() == 6);
    CHECK(last.chunk == 1u);
    CHECK(last.row == 0u);
}

TEST_CASE("Remove() fill the hole with the last row")
{
    Env env;
    Archetype a = env.Make(0, kDefaultChunkBytes, {env.pos});

    std::vector<EntityRecord> records;
    for (std::uint32_t i = 0; i < 3; ++i)
    {
        records.push_back(a.Allocate(E(i)));
        a.GetChunk(0).Components<Position>(env.pos)[i] = Position{static_cast<float>(i), 0.0f, 0.0f};
    }

    const EntityId moved = a.Remove(records[0]);

    CHECK(moved == E(2));
    CHECK(a.EntityCount() == 2);
    CHECK(a.GetChunk(0).Entities()[0] == E(2));
    CHECK(a.GetChunk(0).Components<Position>(env.pos)[0].x == 2.0f);   // datas followed
}

TEST_CASE("Remove() on the last row doesn't move anything")
{
    Env env;
    Archetype a = env.Make(0, kDefaultChunkBytes, {env.pos});
    a.Allocate(E(0));
    const EntityRecord last = a.Allocate(E(1));

    CHECK(a.Remove(last) == kNullEntity);
    CHECK(a.EntityCount() == 1);
}

TEST_CASE("Remove() ignore a wrong location")
{
    Env env;
    Archetype a = env.Make(4, kDefaultChunkBytes, {env.pos});
    const EntityRecord good = a.Allocate(E(0));

    EntityRecord wrongArchetype = good;
    wrongArchetype.archetype = 9;
    EntityRecord wrongChunk = good;
    wrongChunk.chunk = 5;
    EntityRecord wrongRow = good;
    wrongRow.row = 5;

    CHECK(a.Remove(wrongArchetype) == kNullEntity);
    CHECK(a.Remove(wrongChunk) == kNullEntity);
    CHECK(a.Remove(wrongRow) == kNullEntity);
    CHECK(a.Remove(EntityRecord{}) == kNullEntity);
    CHECK(a.EntityCount() == 1);   // Nothing removed
}

TEST_CASE("Allocate() fill the first chunk with space first")
{
    Env env;
    Archetype a = env.Make(0, kSmallChunk, {env.pos});   // 5 per chunk

    std::vector<EntityRecord> records;
    for (std::uint32_t i = 0; i < 10; ++i)
        records.push_back(a.Allocate(E(i)));
    REQUIRE(a.ChunkCount() == 2);

    a.Remove(records[1]);   // 1 free row in the chunk 0
    const EntityRecord reused = a.Allocate(E(99));

    CHECK(reused.chunk == 0u);
    CHECK(reused.row == 4u);
}

TEST_CASE("TrimEmptyChunks only remove last empty chunks")
{
    Env env;
    Archetype a = env.Make(0, kSmallChunk, {env.pos});

    for (std::uint32_t i = 0; i < 15; ++i)
        a.Allocate(E(i));
    REQUIRE(a.ChunkCount() == 3);

    // Emptying chunk 1 (mid) : Should not be removed
    for (int i = 0; i < 5; ++i)
        a.Remove(EntityRecord{0, 1, 0});
    REQUIRE(a.GetChunk(1).IsEmpty());
    a.TrimEmptyChunks();
    CHECK(a.ChunkCount() == 3);

    // Emptying chunk 2 (last) : Trim should remove it then the chunk 1 (it became the last one)
    for (int i = 0; i < 5; ++i)
        a.Remove(EntityRecord{0, 2, 0});
    CHECK(a.ChunkCount() == 3);   // No auto triming
    a.TrimEmptyChunks();
    CHECK(a.ChunkCount() == 1);
    CHECK(a.EntityCount() == 5);

    CHECK(a.Allocate(E(100)).chunk == 1u);   // Make a new chunk again
}

TEST_CASE("MoveRow() add a component : copy existing ones, new one is at zero")
{
    Env env;
    Archetype source = env.Make(1, kDefaultChunkBytes, {env.pos});
    Archetype target = env.Make(2, kDefaultChunkBytes, {env.pos, env.vel});

    std::vector<EntityRecord> records;
    for (std::uint32_t i = 0; i < 3; ++i)
    {
        records.push_back(source.Allocate(E(i)));
        source.GetChunk(0).Components<Position>(env.pos)[i] = Position{static_cast<float>(i + 1), 0.0f, 0.0f};
    }

    const MoveResult result = source.MoveRow(records[0], target);

    // Entity 0 landed in target
    CHECK(result.newRecord.archetype == 2u);
    CHECK(target.EntityCount() == 1);
    CHECK(target.GetChunk(result.newRecord.chunk).Entities()[result.newRecord.row] == E(0));
    const auto positions = target.GetChunk(0).Components<Position>(env.pos);
    const auto velocities = target.GetChunk(0).Components<Velocity>(env.vel);
    CHECK(positions[result.newRecord.row].x == 1.0f);
    CHECK(velocities[result.newRecord.row].x == 0.0f);   // new component : init at zero

    // In source, entity 2 filled the hole left by the moved entity
    CHECK(result.movedInSource == E(2));
    CHECK(source.EntityCount() == 2);
    CHECK(source.GetChunk(0).Entities()[0] == E(2));
    CHECK(source.GetChunk(0).Components<Position>(env.pos)[0].x == 3.0f);
}

TEST_CASE("MoveRow() remove a component : he is removed and the others are keept")
{
    Env env;
    Archetype source = env.Make(1, kDefaultChunkBytes, {env.pos, env.vel});
    Archetype target = env.Make(2, kDefaultChunkBytes, {env.pos});

    const EntityRecord record = source.Allocate(E(0));
    source.GetChunk(0).Components<Position>(env.pos)[0] = Position{5.0f, 6.0f, 7.0f};
    source.GetChunk(0).Components<Velocity>(env.vel)[0] = Velocity{1.0f, 1.0f, 1.0f};

    const MoveResult result = source.MoveRow(record, target);

    CHECK(result.movedInSource == kNullEntity);   // it was the last row
    CHECK(source.EntityCount() == 0);
    const auto positions = target.GetChunk(0).Components<Position>(env.pos);
    CHECK(positions[0].x == 5.0f);
    CHECK(positions[0].y == 6.0f);
    CHECK(positions[0].z == 7.0f);
    CHECK(target.GetChunk(0).Components<Velocity>(env.vel).empty());   // Velocity doesn't exist anymore
}

TEST_CASE("MoveRow() to an archetype with a tag")
{
    Env env;
    Archetype source = env.Make(1, kDefaultChunkBytes, {env.health});
    Archetype target = env.Make(2, kDefaultChunkBytes, {env.health, env.flag});

    const EntityRecord record = source.Allocate(E(0));
    source.GetChunk(0).Components<Health>(env.health)[0].value = 77;

    const MoveResult result = source.MoveRow(record, target);

    REQUIRE(result.newRecord.archetype == 2u);
    CHECK(target.Has(env.flag));
    CHECK(target.GetChunk(0).Components<Health>(env.health)[0].value == 77);
}

TEST_CASE("MoveRow() to the same archetype don't do anything")
{
    Env env;
    Archetype a = env.Make(1, kDefaultChunkBytes, {env.pos});
    const EntityRecord record = a.Allocate(E(0));

    const MoveResult result = a.MoveRow(record, a);

    CHECK(result.newRecord.archetype == record.archetype);
    CHECK(result.newRecord.chunk == record.chunk);
    CHECK(result.newRecord.row == record.row);
    CHECK(result.movedInSource == kNullEntity);
    CHECK(a.EntityCount() == 1);
}

TEST_CASE("MoveRow() of an invalid entity fail without modifying anything")
{
    Env env;
    Archetype source = env.Make(1, kDefaultChunkBytes, {env.pos});
    Archetype target = env.Make(2, kDefaultChunkBytes, {env.pos, env.vel});
    source.Allocate(E(0));

    EntityRecord bad;
    bad.archetype = 1;
    bad.chunk = 0;
    bad.row = 9;

    const MoveResult result = source.MoveRow(bad, target);

    CHECK(result.newRecord.archetype == kInvalidArchetype);
    CHECK(result.movedInSource == kNullEntity);
    CHECK(source.EntityCount() == 1);
    CHECK(target.EntityCount() == 0);
}

TEST_CASE("MoveRow() fill the next chunks of the target if the first ones are full")
{
    Env env;
    Archetype source = env.Make(1, kDefaultChunkBytes, {env.health});
    Archetype target = env.Make(2, kSmallChunk, {env.health});   // 100 / (8 + 4) = 8 per chunk
    REQUIRE(target.Layout().Capacity() == 8u);

    std::vector<EntityRecord> records;
    for (std::uint32_t i = 0; i < 10; ++i)
    {
        records.push_back(source.Allocate(E(i)));
        source.GetChunk(0).Components<Health>(env.health)[i].value = static_cast<int>(i);
    }

    // Keep moving the row 0 as the last row replace it
    for (int i = 0; i < 10; ++i)
        REQUIRE(source.MoveRow(EntityRecord{1, 0, 0}, target).newRecord.archetype == 2u);

    CHECK(source.EntityCount() == 0);
    CHECK(target.EntityCount() == 10);
    CHECK(target.ChunkCount() == 2);
}

TEST_CASE("Clone() make an independent clone")
{
    Env env;
    Archetype a = env.Make(5, kSmallChunk, {env.health});
    std::vector<EntityRecord> records;
    for (std::uint32_t i = 0; i < 7; ++i)
    {
        records.push_back(a.Allocate(E(i)));
        a.GetChunk(records.back().chunk).Components<Health>(env.health)[records.back().row].value = static_cast<int>(i);
    }

    Archetype copy = a.Clone();
    CHECK(copy.Id() == 5u);
    CHECK(copy.EntityCount() == 7);
    CHECK(copy.ChunkCount() == a.ChunkCount());
    for (std::uint32_t i = 0; i < 7; ++i)   // Same records refer to the same entities
    {
        const EntityRecord& r = records[i];
        CHECK(copy.GetChunk(r.chunk).Entities()[r.row] == E(i));
        CHECK(copy.GetChunk(r.chunk).Components<Health>(env.health)[r.row].value == static_cast<int>(i));
    }

    copy.Remove(records[0]);   // change the clone
    CHECK(copy.EntityCount() == 6);
    CHECK(a.EntityCount() == 7);   // ...doesn't change the original
    CHECK(a.GetChunk(0).Entities()[0] == E(0));
}

TEST_CASE("Moved archetype's chunks are still working")
{
    Env env;
    std::vector<Archetype> archetypes;
    for (ArchetypeId id = 0; id < 10; ++id)   // multiple vector reallocation
    {
        archetypes.push_back(env.Make(id, kSmallChunk, {env.health}));
        for (std::uint32_t i = 0; i < 7; ++i)
        {
            const EntityRecord r = archetypes.back().Allocate(E(id * 100 + i));
            archetypes.back().GetChunk(r.chunk).Components<Health>(env.health)[r.row].value = static_cast<int>(id);
        }
    }

    for (ArchetypeId id = 0; id < 10; ++id)
    {
        CHECK(archetypes[id].Id() == id);
        CHECK(archetypes[id].EntityCount() == 7);
        CHECK(archetypes[id].GetChunk(0).Components<Health>(env.health)[0].value == static_cast<int>(id));
        const EntityRecord extra = archetypes[id].Allocate(E(9999));   // The layout is still working
        CHECK(extra.archetype == id);
    }
}

TEST_CASE("Affecting an archetype by a move free properly the previous content")
{
    Env env;
    Archetype a = env.Make(1, kSmallChunk, {env.health});
    Archetype b = env.Make(2, kSmallChunk, {env.pos});
    a.Allocate(E(0));
    b.Allocate(E(1));
    b.Allocate(E(2));

    b = std::move(a);   // Old layout and chunks of b are freed

    CHECK(b.Id() == 1u);
    CHECK(b.EntityCount() == 1);
    CHECK(b.Has(env.health));
    CHECK_FALSE(b.Has(env.pos));
    CHECK(b.Allocate(E(5)).archetype == 1u);   // b is still working
}