#pragma once
#include <cstdint>
#include <functional>

#include <Koji/ECS/System.h>

namespace Koji::ECS
{
    struct TimeSettings
    {
        double fixedDelta = 1.0 / 60.0;   // fixed delta, for physics
        double maxDelta   = 0.25;         // dt ceiling, avoid huge value after a freeze
    };

    // Systems needing a delta time declare Require<TimeSystem>()
    class TimeSystem : public System
    {
    public:
        // Clock: always return seconds
        using ClockFn = std::function<double()>;

        // Empty clock = default clock (std::chrono::steady_clock).
        explicit TimeSystem(TimeSettings settings = {}, ClockFn clock = {});

        // Save the starting time
        // Return false if the settings are wrong (fixedDelta of maxDelta <= 0).
        bool Init(SystemContext& ctx) override;

        // Calculate this frame delta time and fixed steps
        void Update(SystemContext& ctx) override;

        float DeltaTime() const { return static_cast<float>(deltaTime_); }
        float FixedDelta() const { return static_cast<float>(settings_.fixedDelta); }
        int FixedSteps() const { return fixedSteps_; }          // fixed step to execute at this frame
        double TotalTime() const { return totalTime_; }          // delta times sum, total play time
        std::uint64_t FrameCount() const { return frameCount_; }
        float Fps() const { return deltaTime_ > 0.0 ? static_cast<float>(1.0 / deltaTime_) : 0.0f; }

    private:
        TimeSettings settings_;
        ClockFn clock_;
        double lastTime_ = 0.0;
        double deltaTime_ = 0.0;
        double totalTime_ = 0.0;
        double accumulator_ = 0.0;
        int fixedSteps_ = 0;
        std::uint64_t frameCount_ = 0;
    };
}