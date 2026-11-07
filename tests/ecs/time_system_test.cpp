#include <doctest/doctest.h>

#include <Koji/ECS/SystemScheduler.h>
#include <Koji/ECS/Systems/TimeSystem.h>

using namespace Koji::ECS;

// Use value of the power of 2 (0.25, 0.625, 0.125...),
// they are exact in binary so the float comparisons are safes
namespace
{
    TimeSettings TestSettings()
    {
        TimeSettings s;
        s.fixedDelta = 0.25;
        s.maxDelta = 1.0;
        return s;
    }

    // Correct way to use: get the system during init and keep the pointer
    struct Reader : System
    {
        SystemDesc Describe() const override
        {
            SystemDesc d;
            d.Require<TimeSystem>();
            return d;
        }
        bool Init(SystemContext& ctx) override
        {
            time = ctx.GetSystem<TimeSystem>();
            return time != nullptr;
        }
        void Update(SystemContext&) override { seenDt = time->DeltaTime(); }

        const TimeSystem* time = nullptr;
        float seenDt = -1.0f;
    };
}


TEST_CASE("First delta time is from the init")
{
    double now = 100.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;

    REQUIRE(time.Init(ctx));
    now = 100.5;
    time.Update(ctx);

    CHECK(time.DeltaTime() == 0.5f);
}

TEST_CASE("Delta time keep track of the clock")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 0.25;
    time.Update(ctx);
    CHECK(time.DeltaTime() == 0.25f);

    now = 0.75;
    time.Update(ctx);
    CHECK(time.DeltaTime() == 0.5f);
}

TEST_CASE("un deltaTime trop grand est plafonné à maxDelta")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 5.0;   // 5s break but a 1s ceiling
    time.Update(ctx);
    CHECK(time.DeltaTime() == 1.0f);
}

TEST_CASE("A clock going backward give a null value (0.0)")
{
    double now = 10.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 9.0;
    time.Update(ctx);
    CHECK(time.DeltaTime() == 0.0f);
    CHECK(time.Fps() == 0.0f);
}

TEST_CASE("FixedSteps accumulate the time and keep the remaining for the next frame")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });   // 0.25 fixed step
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 0.625;   // +0.625 : 2 step, remain 0.125
    time.Update(ctx);
    CHECK(time.FixedSteps() == 2);

    now = 1.25;    // +0.625 : remain 0.75 -> 3 pas, remain 0
    time.Update(ctx);
    CHECK(time.FixedSteps() == 3);

    now = 1.375;   // +0.125 : not enough for a step
    time.Update(ctx);
    CHECK(time.FixedSteps() == 0);

    now = 6.375;   // +5 cut to 1.0 : remain 0.125 + 1.0 -> 4 step, remain 0.125
    time.Update(ctx);
    CHECK(time.FixedSteps() == 4);
}

TEST_CASE("TotalTime and FrameCount are correct")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 0.25;
    time.Update(ctx);
    now = 0.75;
    time.Update(ctx);

    CHECK(time.TotalTime() == 0.75);
    CHECK(time.FrameCount() == 2);
}

TEST_CASE("Fps is the opposite of deltaTime")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));

    now = 0.25;
    time.Update(ctx);
    CHECK(time.Fps() == 4.0f);
}

TEST_CASE("Init reset counters")
{
    double now = 0.0;
    TimeSystem time(TestSettings(), [&now] { return now; });
    SystemContext ctx;
    REQUIRE(time.Init(ctx));
    now = 0.5;
    time.Update(ctx);
    REQUIRE(time.FrameCount() == 1);

    REQUIRE(time.Init(ctx));
    CHECK(time.FrameCount() == 0);
    CHECK(time.TotalTime() == 0.0);
    CHECK(time.DeltaTime() == 0.0f);
    CHECK(time.FixedSteps() == 0);
}

TEST_CASE("Init reject invalid settings")
{
    SystemContext ctx;

    TimeSettings noFixed;
    noFixed.fixedDelta = 0.0;
    TimeSystem a(noFixed);
    CHECK_FALSE(a.Init(ctx));

    TimeSettings noMax;
    noMax.maxDelta = -1.0;
    TimeSystem b(noMax);
    CHECK_FALSE(b.Init(ctx));
}

TEST_CASE("Real clock give a positive or null value")
{
    TimeSystem time;   // default clock : steady_clock
    SystemContext ctx;
    REQUIRE(time.Init(ctx));
    time.Update(ctx);
    time.Update(ctx);

    CHECK(time.DeltaTime() >= 0.0f);
    CHECK(time.FrameCount() == 2);
}

TEST_CASE("A system requiering the TimeSystem is updated after it")
{
    double now = 0.0;
    SystemRegistry reg;
    auto* reader = reg.AddSystem<Reader>();   // added before TimeSystem
    reg.AddSystem<TimeSystem>(TestSettings(), [&now] { return now; });

    SystemScheduler sched;
    REQUIRE(sched.Build(reg));

    SystemContext ctx;
    REQUIRE(sched.Init(ctx));
    now = 0.25;
    sched.Run(ctx);

    CHECK(reader->seenDt == 0.25f);   // 0 if Reader ran before TimeSystem
    CHECK(sched.Close(ctx));
}