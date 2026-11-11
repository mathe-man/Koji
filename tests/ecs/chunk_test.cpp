#include <doctest/doctest.h>

#include <cstdint>
#include <initializer_list>
#include <utility>
#include <vector>

#include <Koji/ECS/Chunk.h>

using namespace Koji::ECS;

namespace
{
    struct Position { float x, y, z; };
    struct Health { int value; };
    struct alignas(16) Vec4 { float v[4]; };
    struct Flag {};

    enum class C { Pos, Health, Vec, Flag };

    // Register + test layout.
    // Constructor create a layout based on IDs (Id(C::Pos), ...)
    struct Env
    {
        ComponentRegistry reg;
        ComponentId pos = reg.RegisterType<Position>();
        ComponentId health = reg.RegisterType<Health>();
        ComponentId vec = reg.RegisterType<Vec4>();
        ComponentId flag = reg.RegisterType<Flag>();
        std::optional<ChunkLayout> layout;

        Env(std::size_t chunkBytes, std::initializer_list<C> kinds)
        {
            std::vector<ComponentInfo> infos;
            for (C kind : kinds)
                infos.push_back(*reg.Info(Id(kind)));
            layout = ChunkLayout::Create(chunkBytes, infos);
        }

        ComponentId Id(C kind) const
        {
            switch (kind)
            {
                case C::Pos:    return pos;
                case C::Health: return health;
                case C::Vec:    return vec;
                case C::Flag:   return flag;
            }
            return kInvalidComponent;
        }
    };

    EntityId E(std::uint32_t index) { return EntityId{index, 0}; }
}


TEST_CASE("A new chunk is empty")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);

    CHECK(chunk.Count() == 0u);
    CHECK(chunk.IsEmpty());
    CHECK_FALSE(chunk.IsFull());
    CHECK(chunk.Capacity() == env.layout->Capacity());
    CHECK(chunk.Entities().empty());
}

TEST_CASE("Push add new following lines and store the IDs")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);

    CHECK(chunk.Push(E(10)) == 0u);
    CHECK(chunk.Push(E(11)) == 1u);
    CHECK(chunk.Push(E(12)) == 2u);

    CHECK(chunk.Count() == 3u);
    const auto entities = chunk.Entities();
    REQUIRE(entities.size() == 3);
    CHECK(entities[0] == E(10));
    CHECK(entities[1] == E(11));
    CHECK(entities[2] == E(12));
}

TEST_CASE("Push set the new line's components to zero")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);

    chunk.Push(E(1));
    chunk.Components<Position>(env.pos)[0] = Position{1.0f, 2.0f, 3.0f};
    chunk.SwapRemove(0);

    chunk.Push(E(2));   // Reuse the row 0, old component data should not survive
    const auto positions = chunk.Components<Position>(env.pos);
    REQUIRE(positions.size() == 1);
    CHECK(positions[0].x == 0.0f);
    CHECK(positions[0].y == 0.0f);
    CHECK(positions[0].z == 0.0f);
}

TEST_CASE("Components<T> give a typed column of Count() elements")
{
    Env env(kDefaultChunkBytes, {C::Pos, C::Health});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);

    for (std::uint32_t i = 0; i < 4; ++i)
        chunk.Push(E(i));

    auto positions = chunk.Components<Position>(env.pos);
    auto healths = chunk.Components<Health>(env.health);
    REQUIRE(positions.size() == 4);
    REQUIRE(healths.size() == 4);

    for (std::size_t i = 0; i < 4; ++i)
    {
        positions[i] = Position{static_cast<float>(i), 0.0f, 0.0f};
        healths[i].value = static_cast<int>(100 + i);
    }

    const Chunk& view = chunk;
    CHECK(view.Components<Position>(env.pos)[3].x == 3.0f);
    CHECK(view.Components<Health>(env.health)[2].value == 102);
}

TEST_CASE("Components<T> return an empty span if the type don't match")
{
    Env env(kDefaultChunkBytes, {C::Pos, C::Flag});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));

    CHECK(chunk.Components<Health>(env.pos).empty());        // Health => Not the same size
    CHECK(chunk.Components<Position>(env.health).empty());   // Not in this chunk
    CHECK(chunk.Components<Flag>(env.flag).empty());         // A tag doesn't take memory, no column for him
    CHECK(chunk.Components<Position>(kInvalidComponent).empty());
}

TEST_CASE("Columns start at an aligned address")
{
    Env env(kDefaultChunkBytes, {C::Pos, C::Vec});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));

    for (const ChunkColumn& column : env.layout->Columns())
    {
        const auto address = reinterpret_cast<std::uintptr_t>(chunk.Column(column.id));
        CHECK(address % column.align == 0);
    }
}

TEST_CASE("Column() and ComponentAt() return nullptr if the component is absent, a tag, or the row is out of range/not valid")
{
    Env env(kDefaultChunkBytes, {C::Pos, C::Flag});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));

    CHECK(chunk.Column(env.pos) != nullptr);
    CHECK(chunk.Column(env.health) == nullptr);   // absent
    CHECK(chunk.Column(env.flag) == nullptr);     // tag

    CHECK(chunk.ComponentAt(env.pos, 0) != nullptr);
    CHECK(chunk.ComponentAt(env.pos, 1) == nullptr);   // row >= Count()
    CHECK(chunk.ComponentAt(env.flag, 0) == nullptr);
}

TEST_CASE("ComponentAt() point to the right row")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));
    chunk.Push(E(1));

    const auto positions = chunk.Components<Position>(env.pos);
    CHECK(chunk.ComponentAt(env.pos, 0) == &positions[0]);
    CHECK(chunk.ComponentAt(env.pos, 1) == &positions[1]);
}

TEST_CASE("SwapRemove() move the last row into the empty one")
{
    Env env(kDefaultChunkBytes, {C::Pos, C::Health});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);

    for (std::uint32_t i = 0; i < 3; ++i)
    {
        chunk.Push(E(i));
        chunk.Components<Position>(env.pos)[i] = Position{static_cast<float>(i), 0.0f, 0.0f};
        chunk.Components<Health>(env.health)[i].value = static_cast<int>(10 * (i + 1));
    }

    const EntityId moved = chunk.SwapRemove(0);   // remove row 0, row 2 should take it

    CHECK(moved == E(2));
    CHECK(chunk.Count() == 2u);
    const auto entities = chunk.Entities();
    CHECK(entities[0] == E(2));
    CHECK(entities[1] == E(1));
    CHECK(chunk.Components<Position>(env.pos)[0].x == 2.0f);    // components moved too
    CHECK(chunk.Components<Health>(env.health)[0].value == 30);
    CHECK(chunk.Components<Health>(env.health)[1].value == 20); // row 1 didn't changed
}

TEST_CASE("SwapRemove() on the last row doesn't move anything")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));
    chunk.Push(E(1));

    CHECK(chunk.SwapRemove(1) == kNullEntity);
    CHECK(chunk.Count() == 1u);
    CHECK(chunk.Entities()[0] == E(0));

    CHECK(chunk.SwapRemove(0) == kNullEntity);
    CHECK(chunk.IsEmpty());
}

TEST_CASE("SwapRemove() out of range have no effect")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));

    CHECK(chunk.SwapRemove(5) == kNullEntity);
    CHECK(chunk.Count() == 1u);
}

TEST_CASE("Push() return kInvalidRow when the chunk is full")
{
    Env env(100, {C::Pos});   // 20 bytes per entity : 5 entity per chunk
    REQUIRE(env.layout.has_value());
    REQUIRE(env.layout->Capacity() == 5u);
    Chunk chunk(*env.layout);

    for (std::uint32_t i = 0; i < 5; ++i)
        CHECK(chunk.Push(E(i)) == i);

    CHECK(chunk.IsFull());
    CHECK(chunk.Push(E(99)) == Chunk::kInvalidRow);
    CHECK(chunk.Count() == 5u);

    chunk.SwapRemove(2);
    CHECK_FALSE(chunk.IsFull());
    CHECK(chunk.Push(E(99)) == 4u);
}

TEST_CASE("Clone() make an independant copy")
{
    Env env(kDefaultChunkBytes, {C::Health});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(7));
    chunk.Components<Health>(env.health)[0].value = 42;

    Chunk copy = chunk.Clone();
    CHECK(copy.Count() == 1u);
    CHECK(copy.Entities()[0] == E(7));
    CHECK(copy.Components<Health>(env.health)[0].value == 42);

    copy.Components<Health>(env.health)[0].value = 1;   // modifying the clone...
    chunk.Push(E(8));
    CHECK(chunk.Components<Health>(env.health)[0].value == 42);   // ...doesn't change the original
    CHECK(copy.Count() == 1u);
}

TEST_CASE("Clear() clear the chunk")
{
    Env env(kDefaultChunkBytes, {C::Pos});
    REQUIRE(env.layout.has_value());
    Chunk chunk(*env.layout);
    chunk.Push(E(0));
    chunk.Push(E(1));

    chunk.Clear();
    CHECK(chunk.IsEmpty());
    CHECK(chunk.Push(E(5)) == 0u);
}

TEST_CASE("A moved chunk move is datas and is no more usable")
{
    Env env(kDefaultChunkBytes, {C::Health});
    REQUIRE(env.layout.has_value());
    Chunk a(*env.layout);
    a.Push(E(3));
    a.Components<Health>(env.health)[0].value = 42;

    Chunk b(std::move(a));
    CHECK(b.Count() == 1u);
    CHECK(b.Components<Health>(env.health)[0].value == 42);
    CHECK(a.Count() == 0u);                      // NOLINT
    CHECK(a.Push(E(9)) == Chunk::kInvalidRow);   // unusable
}

TEST_CASE("Chunks are still working after a vector reallocation")
{
    Env env(kDefaultChunkBytes, {C::Health});
    REQUIRE(env.layout.has_value());

    std::vector<Chunk> chunks;
    for (std::uint32_t i = 0; i < 20; ++i)   // multiple reallocations
    {
        chunks.emplace_back(*env.layout);
        chunks.back().Push(E(i));
        chunks.back().Components<Health>(env.health)[0].value = static_cast<int>(i);
    }

    for (std::uint32_t i = 0; i < 20; ++i)
    {
        CHECK(chunks[i].Entities()[0] == E(i));
        CHECK(chunks[i].Components<Health>(env.health)[0].value == static_cast<int>(i));
    }
}