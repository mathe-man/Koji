#include <doctest/doctest.h>

#include <cstdint>
#include <unordered_set>
#include <vector>

#include <Koji/ECS/EntityManager.h>

using namespace Koji::ECS;


TEST_CASE("Create new and alive entites")
{
    EntityManager em;
    const EntityId a = em.Create();
    const EntityId b = em.Create();
    const EntityId c = em.Create();

    CHECK(a != b);
    CHECK(b != c);
    CHECK(a != c);
    CHECK(a.index == 0u);
    CHECK(a.generation == 0u);
    CHECK(em.IsAlive(a));
    CHECK(em.IsAlive(b));
    CHECK(em.IsAlive(c));
    CHECK(em.AliveCount() == 3);
}

TEST_CASE("Destroy kill the entity one time")
{
    EntityManager em;
    const EntityId a = em.Create();

    CHECK(em.Destroy(a));
    CHECK_FALSE(em.IsAlive(a));
    CHECK_FALSE(em.Destroy(a));
    CHECK(em.AliveCount() == 0);
}

TEST_CASE("A recycled slot's generation change and previous IDs are dead")
{
    EntityManager em;
    const EntityId a = em.Create();
    REQUIRE(em.Destroy(a));
    const EntityId b = em.Create();   // reuse a's case

    CHECK(b.index == a.index);
    CHECK(b.generation == a.generation + 1);
    CHECK(a != b);
    CHECK_FALSE(em.IsAlive(a));
    CHECK(em.IsAlive(b));

    // Old id cannot destroy the new one
    CHECK_FALSE(em.Destroy(a));
    CHECK(em.IsAlive(b));
}

TEST_CASE("kNullEntity and random IDs are not alive")
{
    EntityManager em;
    em.Create();

    CHECK(kNullEntity.IsNull());
    CHECK_FALSE(em.IsAlive(kNullEntity));
    CHECK_FALSE(em.Destroy(kNullEntity));
    CHECK_FALSE(em.IsAlive(EntityId{42, 0}));
    CHECK_FALSE(em.IsAlive(EntityId{0, 7}));
}

TEST_CASE("New entity don't have a location yet")
{
    EntityManager em;
    const EntityId a = em.Create();

    const auto record = em.Locate(a);
    REQUIRE(record.has_value());
    CHECK(record->archetype == kInvalidArchetype);
}

TEST_CASE("SetLocation then Locate return the good location")
{
    EntityManager em;
    const EntityId a = em.Create();

    EntityRecord where;
    where.archetype = 3;
    where.chunk = 1;
    where.row = 17;
    REQUIRE(em.SetLocation(a, where));

    const auto record = em.Locate(a);
    REQUIRE(record.has_value());
    CHECK(record->archetype == 3u);
    CHECK(record->chunk == 1u);
    CHECK(record->row == 17u);
}

TEST_CASE("Locate and SetLocation reject dead entities")
{
    EntityManager em;
    const EntityId a = em.Create();
    REQUIRE(em.Destroy(a));

    CHECK_FALSE(em.Locate(a).has_value());
    CHECK_FALSE(em.SetLocation(a, EntityRecord{}));
}

TEST_CASE("A recycled slot doesn't have an emplacement")
{
    EntityManager em;
    const EntityId a = em.Create();
    EntityRecord where;
    where.archetype = 5;
    REQUIRE(em.SetLocation(a, where));
    REQUIRE(em.Destroy(a));

    const EntityId b = em.Create();   // même case
    const auto record = em.Locate(b);
    REQUIRE(record.has_value());
    CHECK(record->archetype == kInvalidArchetype);
}

TEST_CASE("IDs are still consistent after a lot of creation and deletion")
{
    EntityManager em;
    std::vector<EntityId> created;
    for (int i = 0; i < 1000; ++i)
        created.push_back(em.Create());

    std::vector<EntityId> dead;
    std::vector<EntityId> kept;
    for (std::size_t i = 0; i < created.size(); ++i)
    {
        if (i % 2 == 0)
        {
            REQUIRE(em.Destroy(created[i]));
            dead.push_back(created[i]);
        }
        else
        {
            kept.push_back(created[i]);
        }
    }
    for (int i = 0; i < 500; ++i)
        kept.push_back(em.Create());   // Recycle the 500 freed slots

    CHECK(em.AliveCount() == 1000);
    for (const EntityId& e : kept)
        CHECK(em.IsAlive(e));
    for (const EntityId& e : dead)
        CHECK_FALSE(em.IsAlive(e));

    // Not a single alive id is doubled
    std::unordered_set<std::uint64_t> unique;
    for (const EntityId& e : kept)
        unique.insert((static_cast<std::uint64_t>(e.index) << 32) | e.generation);
    CHECK(unique.size() == kept.size());
}