#include <doctest/doctest.h>

#include <typeindex>
#include <vector>

#include <Koji/ECS/SystemContext.h>

using namespace Koji::ECS;

namespace
{
    struct TimeLike : System {};

    struct PhysicsLike : System
    {
        SystemDesc Describe() const override
        {
            SystemDesc d;
            d.Require<TimeLike>().Read(1).Write(2);
            return d;
        }
    };

    struct WithArgs : System
    {
        WithArgs(int a, float b) : a(a), b(b) {}
        int a;
        float b;
    };

    // Write is id in a log once destroyed
    struct Tracked : System
    {
        Tracked(std::vector<int>& log, int id) : log(log), id(id) {}
        ~Tracked() override { log.push_back(id); }
        std::vector<int>& log;
        int id;
    };
    struct TrackedA : Tracked { using Tracked::Tracked; };
    struct TrackedB : Tracked { using Tracked::Tracked; };
    struct TrackedC : Tracked { using Tracked::Tracked; };
}


TEST_CASE("AddSystem return the system and GetSystem find it")
{
    SystemRegistry reg;
    auto* time = reg.AddSystem<TimeLike>();
    REQUIRE(time != nullptr);
    CHECK(reg.GetSystem<TimeLike>() == time);
    CHECK(reg.HasSystem<TimeLike>());
    CHECK(reg.Count() == 1);
}

TEST_CASE("GetSystem return nullptr for non existing system")
{
    SystemRegistry reg;
    CHECK(reg.GetSystem<TimeLike>() == nullptr);
    CHECK_FALSE(reg.HasSystem<TimeLike>());
}

TEST_CASE("Only one system per type")
{
    SystemRegistry reg;
    reg.AddSystem<TimeLike>();
    CHECK(reg.AddSystem<TimeLike>() == nullptr);
    CHECK(reg.Count() == 1);
}

TEST_CASE("AddSystem work with constructor arguments")
{
    SystemRegistry reg;
    auto* s = reg.AddSystem<WithArgs>(7, 2.5f);
    REQUIRE(s != nullptr);
    CHECK(s->a == 7);
    CHECK(s->b == 2.5f);
}

TEST_CASE("RemoveSystem remove the system")
{
    SystemRegistry reg;
    reg.AddSystem<TimeLike>();
    reg.AddSystem<PhysicsLike>();

    CHECK(reg.RemoveSystem<TimeLike>());
    CHECK_FALSE(reg.RemoveSystem<TimeLike>());
    CHECK(reg.GetSystem<TimeLike>() == nullptr);
    CHECK(reg.Count() == 1);
    CHECK(reg.HasSystem<PhysicsLike>());
}

TEST_CASE("All keep the order in which systems were added")
{
    SystemRegistry reg;
    reg.AddSystem<PhysicsLike>();
    reg.AddSystem<TimeLike>();
    REQUIRE(reg.All().size() == 2);
    CHECK(reg.All()[0].get() == reg.GetSystem<PhysicsLike>());
    CHECK(reg.All()[1].get() == reg.GetSystem<TimeLike>());
}

TEST_CASE("Describe declare required systems and components")
{
    PhysicsLike physics;
    const SystemDesc desc = physics.Describe();
    REQUIRE(desc.requiredSystems.size() == 1);
    CHECK(desc.requiredSystems[0] == std::type_index(typeid(TimeLike)));
    CHECK(desc.reads == std::vector<ComponentId>{1});
    CHECK(desc.writes == std::vector<ComponentId>{2});
}

TEST_CASE("Find retrieve a system by is type")
{
    SystemRegistry reg;
    auto* time = reg.AddSystem<TimeLike>();
    CHECK(reg.Find(std::type_index(typeid(TimeLike))) == time);
    CHECK(reg.Find(std::type_index(typeid(PhysicsLike))) == nullptr);
}

TEST_CASE("Systems are destroyed in the reverse order")
{
    std::vector<int> log;
    {
        SystemRegistry reg;
        reg.AddSystem<TrackedA>(log, 1);
        reg.AddSystem<TrackedB>(log, 2);
        reg.AddSystem<TrackedC>(log, 3);
    }
    const std::vector<int> expected{3, 2, 1};   // hors de CHECK : une virgule y couperait l'argument
    CHECK(log == expected);
}

TEST_CASE("SystemContext::GetSystem use the register")
{
    SystemRegistry reg;
    auto* time = reg.AddSystem<TimeLike>();

    SystemContext ctx;
    CHECK(ctx.GetSystem<TimeLike>() == nullptr);   // pas de registre branché
    ctx.systems = &reg;
    CHECK(ctx.GetSystem<TimeLike>() == time);
}