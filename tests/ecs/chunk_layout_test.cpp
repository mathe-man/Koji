#include <doctest/doctest.h>

#include <initializer_list>
#include <vector>

#include <Koji/ECS/ChunkLayout.h>

using namespace Koji::ECS;

namespace
{
    struct Position { float x, y, z; };
    struct Velocity { float x, y, z; };
    struct alignas(16) Vec4 { float v[4]; };
    struct Flag {};

    std::vector<ComponentInfo> Infos(const ComponentRegistry& reg, std::initializer_list<ComponentId> ids)
    {
        std::vector<ComponentInfo> infos;
        for (ComponentId id : ids)
            infos.push_back(*reg.Info(id));
        return infos;
    }

    // Check that columns are correct : aligned columns, without overlaps
    void CheckColumnsAreSound(const ChunkLayout& layout)
    {
        struct Range { std::size_t begin, end; };
        std::vector<Range> ranges;
        ranges.push_back({0, std::size_t{layout.Capacity()} * sizeof(EntityId)});

        for (const ChunkColumn& column : layout.Columns())
        {
            if (column.size == 0)
                continue;
            CHECK(column.offset % column.align == 0);
            ranges.push_back({column.offset, column.offset + std::size_t{layout.Capacity()} * column.size});
        }

        for (std::size_t i = 0; i < ranges.size(); ++i)
        {
            CHECK(ranges[i].end <= layout.ChunkBytes());
            for (std::size_t j = i + 1; j < ranges.size(); ++j)
                CHECK((ranges[i].end <= ranges[j].begin || ranges[j].end <= ranges[i].begin));
        }
    }
}


TEST_CASE("Create() calculate the capacity, the chunk is perfectly filled")
{
    ComponentRegistry reg;
    const ComponentId pos = reg.RegisterType<Position>();
    const ComponentId vel = reg.RegisterType<Velocity>();

    // 8 (EntityId) + 12 + 12 = 32 bytes per entity : 16384 / 32 = 512
    const auto layout = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos, vel}));
    REQUIRE(layout.has_value());
    CHECK(layout->Capacity() == 512u);
    CHECK(layout->ChunkBytes() == kDefaultChunkBytes);
    CheckColumnsAreSound(*layout);
}

TEST_CASE("Create() allow archetypes without components : only EntityId")
{
    const auto layout = ChunkLayout::Create(kDefaultChunkBytes, {});
    REQUIRE(layout.has_value());
    CHECK(layout->Capacity() == kDefaultChunkBytes / sizeof(EntityId));
    CHECK(layout->Columns().empty());
}

TEST_CASE("Signature contains the layout components")
{
    ComponentRegistry reg;
    const ComponentId pos = reg.RegisterType<Position>();
    const ComponentId vel = reg.RegisterType<Velocity>();
    const ComponentId vec = reg.RegisterType<Vec4>();

    const auto layout = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos, vec}));
    REQUIRE(layout.has_value());
    CHECK(layout->Signature().test(pos));
    CHECK(layout->Signature().test(vec));
    CHECK_FALSE(layout->Signature().test(vel));
    CHECK(layout->Signature().count() == 2);

    CHECK(layout->Has(pos));
    CHECK_FALSE(layout->Has(vel));
    CHECK(layout->FindColumn(vel) == nullptr);
    CHECK(layout->FindColumn(kInvalidComponent) == nullptr);
}

TEST_CASE("Columns follow the alignment of their components")
{
    ComponentRegistry reg;
    const ComponentId pos = reg.RegisterType<Position>();   // align on 4
    const ComponentId vec = reg.RegisterType<Vec4>();       // align on 16

    const auto layout = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos, vec}));
    REQUIRE(layout.has_value());
    CheckColumnsAreSound(*layout);
    CHECK(layout->FindColumn(vec)->offset % 16 == 0);
}

TEST_CASE("Padding due to alignment can reduce capacity")
{
    ComponentRegistry reg;
    const ComponentId vec = reg.RegisterType<Vec4>();

    // 24 bytes per entity : 120 / 24 = 5, but with padding 5 lines require 128 bytes
    const auto layout = ChunkLayout::Create(120, Infos(reg, {vec}));
    REQUIRE(layout.has_value());
    CHECK(layout->Capacity() == 4u);
    CheckColumnsAreSound(*layout);
}

TEST_CASE("A tag doesn't use memory")
{
    ComponentRegistry reg;
    const ComponentId pos = reg.RegisterType<Position>();
    const ComponentId flag = reg.RegisterType<Flag>();

    const auto withTag = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos, flag}));
    const auto withoutTag = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos}));
    REQUIRE(withTag.has_value());
    REQUIRE(withoutTag.has_value());

    CHECK(withTag->Capacity() == withoutTag->Capacity());
    CHECK(withTag->Has(flag));                       // His in the layout
    CHECK(withTag->FindColumn(flag)->size == 0u);    // But doesn't use memory
    CheckColumnsAreSound(*withTag);
}

TEST_CASE("The chuck alignement is based on the more exigeant component")
{
    const ComponentInfo big{0, "A128", 128, 128};
    const auto layout = ChunkLayout::Create(kDefaultChunkBytes, {big});
    REQUIRE(layout.has_value());
    CHECK(layout->Alignment() == 128u);
    CHECK(layout->Capacity() == 120u);   // (8 + 128) bytes per entity, padding included : 120 is enough
    CheckColumnsAreSound(*layout);

    ComponentRegistry reg;
    const ComponentId pos = reg.RegisterType<Position>();
    const auto small = ChunkLayout::Create(kDefaultChunkBytes, Infos(reg, {pos}));
    REQUIRE(small.has_value());
    CHECK(small->Alignment() == kChunkMinAlignment);
}

TEST_CASE("Create refuse un composant qui ne tient pas dans le bloc")
{
    const ComponentInfo huge{0, "Huge", 20000, 4};
    CHECK_FALSE(ChunkLayout::Create(kDefaultChunkBytes, {huge}).has_value());
    CHECK_FALSE(ChunkLayout::Create(4, {}).has_value());   // A chunk too small even for an EntityId
}

TEST_CASE("Create reject a duplicate and out of range id")
{
    const ComponentInfo a{3, "A", 4, 4};
    CHECK_FALSE(ChunkLayout::Create(kDefaultChunkBytes, {a, a}).has_value());

    const ComponentInfo outOfRange{static_cast<ComponentId>(kMaxComponents), "Out", 4, 4};
    CHECK_FALSE(ChunkLayout::Create(kDefaultChunkBytes, {outOfRange}).has_value());
}