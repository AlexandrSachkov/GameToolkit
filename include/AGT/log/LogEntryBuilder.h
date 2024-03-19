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

#include <assert.h>
#include <span>

namespace AGT {
    class LogEntryBuilder {
    public:
        LogEntryBuilder(std::span<char> buffer) noexcept : m_buffer(buffer) {
            assert(buffer.size() > 1);
        }

        void EndLine(bool force = false) noexcept {
            size_t numLeft = m_buffer.size() - m_offset;
            if (force && !numLeft) {
                m_buffer[m_buffer.size() - 2] = '\n';
            } else {
                m_offset += snprintf(m_buffer.data() + m_offset, m_buffer.size() - m_offset, "\n");
            }
        }

        template<typename... Args>
        void Write(const char* format, Args&&... args) noexcept {
            size_t numLeft = m_buffer.size() - m_offset;
            size_t numRequired = snprintf(
                m_buffer.data() + m_offset, numLeft,
                format, std::forward<Args>(args)...);

            m_offset += std::min(numRequired, numLeft);
        }

        void Write(const char* str) noexcept {
            size_t numLeft = m_buffer.size() - m_offset;
            size_t numRequired = snprintf(
                m_buffer.data() + m_offset, numLeft,
                "%s", str);

            m_offset += std::min(numRequired, numLeft);
        }

        template<typename... Args>
        void WriteLine(const char* format, Args&&... args) noexcept {
            Write(format, std::forward<Args>(args)...);
            EndLine(true);
        }

        void WriteLine(const char* str) noexcept {
            Write(str);
            EndLine(true);
        }

        size_t GetSizeWritten() const noexcept { return m_offset; }
    private:
        LogEntryBuilder(const LogEntryBuilder&) = delete;
        LogEntryBuilder& operator=(const LogEntryBuilder&) = delete;

        std::span<char> m_buffer;
        size_t m_offset{ 0 };
    };
}