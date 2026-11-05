#include <doctest/doctest.h>

#include <string>
#include <vector>

#include <Koji/ECS/SystemScheduler.h>

using namespace Koji::ECS;

namespace
{
    struct Log { std::vector<std::string> events; };

    template <int N, typename... Deps>
    struct Node : System
    {
        explicit Node(Log& log, bool failInit = false, bool failClose = false)
            : log(log), failInit(failInit), failClose(failClose) {}

        SystemDesc Describe() const override
        {
            SystemDesc d;
            (d.Require<Deps>(), ...);
            return d;
        }
        // std::to_string(int value) -> std::string
        bool Init(SystemContext&) override   { log.events.push_back("init " + std::to_string(N));   return !failInit; }
        void Update(SystemContext&) override { log.events.push_back("update " + std::to_string(N)); }
        bool Close(SystemContext&) override  { log.events.push_back("close " + std::to_string(N));  return !failClose; }

        Log& log;
        bool failInit;
        bool failClose;
    };

    using A = Node<1>;
    using B = Node<2, A>;   // B require A
    using C = Node<3, B>;   // C require B
    using D = Node<4>;

    struct MissingDep : System {};
    struct NeedsMissing : System
    {
        SystemDesc Describe() const override { SystemDesc d; d.Require<MissingDep>(); return d; }
    };

    struct CycleY;
    struct CycleX : System { SystemDesc Describe() const override; };
    struct CycleY : System { SystemDesc Describe() const override; };
    SystemDesc CycleX::Describe() const { SystemDesc d; d.Require<CycleY>(); return d; }
    SystemDesc CycleY::Describe() const { SystemDesc d; d.Require<CycleX>(); return d; }

    struct SelfDep : System
    {
        SystemDesc Describe() const override { SystemDesc d; d.Require<SelfDep>(); return d; }
    };

    // Check what the context contain during Init() and Update().
    struct Probe : System
    {
        bool Init(SystemContext& ctx) override   { sawA = ctx.GetSystem<A>() != nullptr; return true; }
        void Update(SystemContext& ctx) override { seenDt = ctx.dt; }
        bool sawA = false;
        float seenDt = 0.0f;
    };

    bool Contains(const std::string& text, const char* part)
    {
        return text.find(part) != std::string::npos;
    }
}

TEST_CASE("Build sort systems depending on their dependencies")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<C>(log);   // Added first voluntary
    reg.AddSystem<A>(log);
    reg.AddSystem<B>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    const std::vector<System*> expected{reg.GetSystem<A>(), reg.GetSystem<B>(), reg.GetSystem<C>()};
    CHECK(sched.Order() == expected);
}

TEST_CASE("Build keep original order when there is no dependency to fix")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<D>(log);
    reg.AddSystem<A>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    const std::vector<System*> expected{reg.GetSystem<D>(), reg.GetSystem<A>()};
    CHECK(sched.Order() == expected);
}

TEST_CASE("Build sort independent systems correctly")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<B>(log);   // require A
    reg.AddSystem<A>(log);
    reg.AddSystem<D>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    // A get before B ; D is independent, stay A and B like in the original order
    const std::vector<System*> expected{reg.GetSystem<A>(), reg.GetSystem<B>(), reg.GetSystem<D>()};
    CHECK(sched.Order() == expected);
}

TEST_CASE("Build signal absent system")
{
    SystemRegistry reg;
    reg.AddSystem<NeedsMissing>();

    SystemScheduler sched;
    CHECK_FALSE(sched.Build(reg));
    CHECK(Contains(sched.Error(), "NeedsMissing"));
    CHECK(Contains(sched.Error(), "MissingDep"));
    CHECK(Contains(sched.Error(), "not registered"));
    CHECK(sched.Order().empty());
}

TEST_CASE("Build signal a dependency loop")
{
    SystemRegistry reg;
    reg.AddSystem<CycleX>();
    reg.AddSystem<CycleY>();

    SystemScheduler sched;
    CHECK_FALSE(sched.Build(reg));
    CHECK(Contains(sched.Error(), "cycle"));
    CHECK(Contains(sched.Error(), "CycleX"));
    CHECK(Contains(sched.Error(), "CycleY"));
}

TEST_CASE("Build signal a system dependent to himself")
{
    SystemRegistry reg;
    reg.AddSystem<SelfDep>();

    SystemScheduler sched;
    CHECK_FALSE(sched.Build(reg));
    CHECK(Contains(sched.Error(), "SelfDep"));
}

TEST_CASE("Build accept empty registry")
{
    SystemRegistry reg;
    SystemScheduler sched;
    CHECK(sched.Build(reg));
    CHECK(sched.Order().empty());

    SystemContext ctx;
    CHECK(sched.Init(ctx));
    CHECK(sched.Close(ctx));
}

TEST_CASE("Init, Run and Close respect the order")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<C>(log);
    reg.AddSystem<A>(log);
    reg.AddSystem<B>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    REQUIRE(sched.Init(ctx));
    sched.Run(ctx);
    sched.Run(ctx);
    CHECK(sched.Close(ctx));

    const std::vector<std::string> expected{
        "init 1", "init 2", "init 3",
        "update 1", "update 2", "update 3",
        "update 1", "update 2", "update 3",
        "close 3", "close 2", "close 1"};
    CHECK(log.events == expected);
}

TEST_CASE("When Init() fail, already inited systems are closed in the reversed order")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);
    reg.AddSystem<B>(log);
    reg.AddSystem<C>(log, true);   // C fail at Init

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    CHECK_FALSE(sched.Init(ctx));
    CHECK_FALSE(sched.IsInitialized());
    CHECK(Contains(sched.Error(), "Init() failed"));

    // C is not closed (he did not inited), A and B are closed, B first.
    const std::vector<std::string> expected{"init 1", "init 2", "init 3", "close 2", "close 1"};
    CHECK(log.events == expected);

    sched.Run(ctx);   // no effect
    CHECK(log.events.size() == expected.size());
}

TEST_CASE("Init without Build fail")
{
    SystemScheduler sched;
    SystemContext ctx;
    CHECK_FALSE(sched.Init(ctx));
    CHECK(Contains(sched.Error(), "Build"));
}

TEST_CASE("Run does nothing before init")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    sched.Run(ctx);
    CHECK(log.events.empty());
}

TEST_CASE("Close have no effect before init")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    CHECK(sched.Close(ctx));
    CHECK(log.events.empty());

    REQUIRE(sched.Init(ctx));
    CHECK(sched.Close(ctx));
    CHECK(sched.Close(ctx));

    const std::vector<std::string> expected{"init 1", "close 1"};
    CHECK(log.events == expected);
}

TEST_CASE("Close call every system even if one fail")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);
    reg.AddSystem<B>(log, false, true);   // B échoue à la fermeture
    reg.AddSystem<C>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    REQUIRE(sched.Init(ctx));
    log.events.clear();

    CHECK_FALSE(sched.Close(ctx));
    const std::vector<std::string> expected{"close 3", "close 2", "close 1"};
    CHECK(log.events == expected);
}

TEST_CASE("Build refuse while systems are inited")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    REQUIRE(sched.Init(ctx));
    CHECK_FALSE(sched.Build(reg));
    CHECK(Contains(sched.Error(), "Close()"));

    REQUIRE(sched.Close(ctx));
    CHECK(sched.Build(reg));
}

TEST_CASE("Context get Dt and give access to the systems")
{
    Log log;
    SystemRegistry reg;
    reg.AddSystem<A>(log);
    auto* probe = reg.AddSystem<Probe>();

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;      // ctx.systems is wired by the scheduler
    ctx.dt = 0.5f;
    REQUIRE(sched.Init(ctx));
    sched.Run(ctx);

    CHECK(probe->sawA);
    CHECK(probe->seenDt == 0.5f);
}