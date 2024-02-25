/*
MIT License

Copyright(c) 2024 Alexandr Sachkov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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