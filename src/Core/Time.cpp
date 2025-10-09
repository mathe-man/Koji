#include "../../include/Koji/Core/Time.h"
using Koji::Time, Koji::DelayTimer;

void Time::Init() {
    s_StartTime = Clock::now();
    s_LastFrameTime = s_StartTime;
    s_DeltaTime = 0.0f;
    s_TotalTime = 0.0f;
}

float Time::DeltaTime() {
    TimePoint now = Clock::now();

    // Time since last frame
    std::chrono::duration<float> frameDuration = now - s_LastFrameTime;
    s_DeltaTime = frameDuration.count();

    // Total time since start
    std::chrono::duration<float> totalTime = now - s_StartTime;
    s_TotalTime = totalTime.count();

    s_LastFrameTime = now;

    return s_DeltaTime;
}

float Time::TotalTime() {
    return s_TotalTime;
}


void DelayTimer::Start() {

    if (m_running) return;

    m_running = true;
    m_thread = std::thread([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds{m_delayMs});
        if (m_running && m_callback)
            m_callback();
    });

    m_thread.detach(); // No need to join later
}
void DelayTimer::Cancel() {
    m_running = false;
}

