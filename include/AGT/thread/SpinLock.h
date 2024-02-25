#pragma once

#include <atomic>
#include <immintrin.h>

namespace AGT {
    class SpinLock {
    public:
        SpinLock() noexcept = default;

        void lock() noexcept {
            for (;;) {
                if (!m_lock.exchange(true, std::memory_order_acquire)) {
                    return;
                }

                while (m_lock.load(std::memory_order_relaxed)) {
                    //https://stackoverflow.com/questions/5833527/how-do-you-use-the-pause-assembly-instruction-in-64-bit-c-code
                    //hint to the compiler that a spin-wait loop is used
                    //TODO assumes SSE2 support. In the future, detect SSE2 or fail
                    _mm_pause();
                }
            }
        }

        bool try_lock() noexcept {
            if (m_lock.load(std::memory_order_relaxed)) {
                return false;
            }

            return !m_lock.exchange(true, std::memory_order_acquire);
        }

        void unlock() noexcept {
            m_lock.store(false, std::memory_order_release);
        }

    private:
        SpinLock(const SpinLock&) noexcept = delete;
        SpinLock& operator=(const SpinLock&) noexcept = delete;

        std::atomic<bool> m_lock { false };
    };
}