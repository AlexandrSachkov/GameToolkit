#pragma once

#include <chrono>

namespace AGT {
    class Timer {
    public:
        static long long GetTimeSinceEpochNs() noexcept {
            auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        }

        float GetDeltaTMs() const noexcept {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<float, std::milli>(now - m_previousTime);
            return duration.count();
        }

        float GetAndUpdateDeltaTMs() noexcept {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<float, std::milli>(now - m_previousTime);
            m_previousTime = now;
            return duration.count();
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_previousTime;
    };
}