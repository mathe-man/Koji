#include "../../include/Koji/Core/Time.h"

using namespace Koji;
using namespace std::chrono;

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
        std::this_thread::sleep_for(milliseconds{m_delayMs});
        if (m_running && m_callback)
            m_callback();
    });

    m_thread.detach(); // No need to join later
}

void RepeatedDelayTimer::Start() {

    if (m_running) return;

    m_running = true;
    m_thread = std::thread([this] {
        auto nextTick = high_resolution_clock::now();

        while (m_running) {
            nextTick += milliseconds(m_delayMs);

            // Launch the callback on a separate thread, avoid blocking this timer
            if (m_running && m_callback)
                std::thread([cb = m_callback, i = m_iteration++] {
                    cb(i);
                }).detach();

            // Sleep until the next scheduled time (even if callback took long)
            std::this_thread::sleep_until(nextTick);
        }
    });

    m_thread.detach(); // No need to join later
}