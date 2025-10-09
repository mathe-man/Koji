#pragma once
#include  <iostream>
#include <chrono>
#include <functional>
#include <thread>

namespace Koji {
    class Time {
    public:
        static void Init();
        static float DeltaTime();

        static float TotalTime();

    private:
        using Clock = std::chrono::high_resolution_clock;
        using TimePoint = std::chrono::time_point<Clock>;

        static inline TimePoint s_StartTime;
        static inline TimePoint s_LastFrameTime;
        static inline float s_DeltaTime = 0.0f;
        static inline float s_TotalTime = 0.0f;
    };

    class DelayTimer {
    public:
        DelayTimer(unsigned int delayMs, std::function<void()> callback)
            : m_delayMs(delayMs), m_callback(std::move(callback)) {}

        void Start();
        void Cancel();

    private:
        unsigned int m_delayMs;
        std::function<void()> m_callback;
        bool m_running = false;
        std::thread m_thread;
    };

    class ScopeTimer {
    public:
        ScopeTimer(std::function<void(double)> callback = nullptr)
            : _callback(callback),
              start(std::chrono::high_resolution_clock::now()) {}

        ~ScopeTimer() {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed= end - start;

            if (_callback)
                _callback(elapsed.count());
        }
    private:
        std::chrono::high_resolution_clock::time_point start;
        std::function<void(double)> _callback;
    };
} // Koji