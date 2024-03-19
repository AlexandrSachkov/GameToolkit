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

#include "ILoggerSink.h"
#include "LogEntryBuilder.h"
#include "LogLevel.h"

#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace AGT {
    template<typename TFormatter>
    class DefaultLogger {
    public:
        static std::unique_ptr<DefaultLogger<TFormatter>> Create(LogLevel maxLevel, size_t maxLineSize, std::span<std::shared_ptr<ILoggerSink>> sinks) {
            auto logger = std::unique_ptr<DefaultLogger<TFormatter>>(new DefaultLogger<TFormatter>());
            if (logger && logger->Init(maxLevel, maxLineSize, sinks)) {
                return logger;
            }

            return nullptr;
        }

        ~DefaultLogger() {
            Flush();
        }

        DefaultLogger(DefaultLogger&& other) noexcept {
            *this = std::move(other);
        }

        DefaultLogger& operator=(DefaultLogger&& other) noexcept {
            m_maxLevel = other.m_maxLevel;
            other.m_maxLevel = LogLevel::Debug;

            m_lineBuffer = std::move(other.m_lineBuffer);
            m_sinks = std::move(other.m_sinks);
        }

        template<typename... Args>
        void Write(
            LogLevel level,
            const char* file,
            const char* function,
            int lineNumber,
            const char* format,
            Args&&... args
        ) {
            std::lock_guard<std::mutex> lock(m_lock);

            if (static_cast<int>(level) > static_cast<int>(m_maxLevel)) {
                return;
            }

            TFormatter formatter;
            LogEntryBuilder builder(m_lineBuffer);

            formatter.Format(
                builder,
                level,
                file,
                function,
                lineNumber,
                format,
                std::forward<Args>(args)...
            );

            for (auto& sink : m_sinks) {
                sink->Write(m_lineBuffer.data(), builder.GetSizeWritten());
            }
        }

        void Flush() {
            std::lock_guard<std::mutex> lock(m_lock);

            for (auto& sink : m_sinks) {
                sink->Flush();
            }
        }

    private:
        DefaultLogger(const DefaultLogger&) = delete;
        DefaultLogger& operator=(const DefaultLogger&) = delete;

        DefaultLogger() noexcept = default;

        bool Init(LogLevel maxLevel, size_t maxLineSize, std::span<std::shared_ptr<ILoggerSink>> sinks) {
            std::lock_guard<std::mutex> lock(m_lock);

            m_maxLevel = maxLevel;
            m_lineBuffer.resize(maxLineSize);

            m_sinks.reserve(sinks.size());
            for (auto& sink : sinks) {
                m_sinks.emplace_back(sink);
            }

            return true;
        }

        std::mutex m_lock;
        LogLevel m_maxLevel{ LogLevel::Debug };
        std::vector<char> m_lineBuffer;
        std::vector<std::shared_ptr<ILoggerSink>> m_sinks;
    };
}