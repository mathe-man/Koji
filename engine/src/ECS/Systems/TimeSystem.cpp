#include <Koji/ECS/Systems/TimeSystem.h>

#include <chrono>

namespace Koji::ECS
{
    namespace
    {
        double SteadySeconds()
        {
            const auto now = std::chrono::steady_clock::now();
            return std::chrono::duration<double>(now.time_since_epoch()).count();
        }
    }

    TimeSystem::TimeSystem(TimeSettings settings, ClockFn clock)
        : settings_(settings), clock_(std::move(clock))
    {
        if (!clock_)
            clock_ = SteadySeconds;
    }

    bool TimeSystem::Init(SystemContext&)
    {
        if (settings_.fixedDelta <= 0.0 || settings_.maxDelta <= 0.0)
            return false;

        // First update's dt will be mesured from here
        lastTime_ = clock_();
        deltaTime_ = 0.0;
        totalTime_ = 0.0;
        accumulator_ = 0.0;
        fixedSteps_ = 0;
        frameCount_ = 0;
        return true;
    }

    void TimeSystem::Update(SystemContext&)
    {
        const double now = clock_();
        double elapsed = now - lastTime_;
        lastTime_ = now;

        if (elapsed < 0.0)
            elapsed = 0.0;                         // Clock going backward = ignored
        if (elapsed > settings_.maxDelta)
            elapsed = settings_.maxDelta;          // Ceiling after a break

        deltaTime_ = elapsed;
        totalTime_ += elapsed;
        ++frameCount_;

        accumulator_ += elapsed;
        fixedSteps_ = 0;
        while (accumulator_ >= settings_.fixedDelta)
        {
            accumulator_ -= settings_.fixedDelta;
            ++fixedSteps_;
        }
    }
}