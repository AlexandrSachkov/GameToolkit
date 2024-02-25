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

#include "../shared/Platform.h"
#include "../time/Timer.h"
#include "../thread/SpinLock.h"
#include "ILoggerSink.h"

#include <memory>
#include <mutex>
#include <span>
#include <vector>

namespace AGT {
    enum class LogLevel {
        Error   = 0,
        Warning = 1,
        InfoV1  = 2, // low noise
        InfoV2  = 3, // medium noise
        InfoV3  = 4, // high noise
        Debug   = 5
    };

    const char* LogLevelToString(LogLevel level) noexcept {
        switch (level) {
        case LogLevel::Error:   return "Error";
        case LogLevel::Warning: return "Warning";
        case LogLevel::InfoV1:  return "InfoV1";
        case LogLevel::InfoV2:  return "InfoV2";
        case LogLevel::InfoV3:  return "InfoV3";
        case LogLevel::Debug:   return "Debug";
        default:
            return "Unknown";
        }
    }

    class LogEntryBuilder {
    public:
        LogEntryBuilder(std::span<char> buffer) noexcept : m_buffer(buffer) {}

        void EndLine() noexcept {
            m_offset += snprintf(m_buffer.data() + m_offset, m_buffer.size() - m_offset, "\n");
        }

        template<typename... Args>
        void Write(const char* format, Args&&... args) noexcept {
            m_offset += snprintf(
                m_buffer.data() + m_offset, m_buffer.size() - m_offset,
                format, std::forward<Args>(args)...);
        }

        void Write(const char* str) noexcept {
            m_offset += snprintf(
                m_buffer.data() + m_offset, m_buffer.size() - m_offset,
                "%s", str);
        }

        template<typename... Args>
        void WriteLine(const char* format, Args&&... args) noexcept {
            Write(format, std::forward<Args>(args)...);
            EndLine();
        }

        void WriteLine(const char* str) noexcept {
            Write(str);
            EndLine();
        }

        size_t GetSize() const noexcept { return m_offset; }
    private:
        LogEntryBuilder(const LogEntryBuilder&) = delete;
        LogEntryBuilder& operator=(const LogEntryBuilder&) = delete;

        std::span<char> m_buffer;
        size_t m_offset{ 0 };
    };

    class Logger {
    public:
        static bool Init(LogLevel maxLevel, size_t maxLineSize, std::span<std::unique_ptr<ILoggerSink>> sinks) noexcept {
            std::lock_guard<SpinLock> lock(s_lock);

            s_maxLevel = maxLevel;
            s_lineBuffer.resize(maxLineSize);
            s_PID = _getpid();

            s_sinks.reserve(sinks.size());

            for (auto& sink : sinks) {
                s_sinks.emplace_back(std::move(sink));
            }

            return true;
        }

        static void Destroy() noexcept {
            Flush();

            std::lock_guard<SpinLock> lock(s_lock);
            s_sinks.clear();
        }

        template<typename... Args>
        static void Write(
            LogLevel level, 
            const char* file, 
            const char* function,
            int lineNumber,
            const char* format, 
            Args&&... args
        ) noexcept {
            std::lock_guard<SpinLock> lock(s_lock);

            if (static_cast<int>(level) > static_cast<int>(s_maxLevel)) {
                return;
            }

            static const char PATH_SEPARATOR =
#ifdef AGT_PLAT_WINDOWS
            '\\';
#elif
            '/';
#endif

            //shortens file path to the file name
            const char* fileName = file;
            const char* fileNamePos = strrchr(file, PATH_SEPARATOR);
            if (fileNamePos) {
                fileName = fileNamePos + 1;
            }

            
            size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
            uint64_t timestampNs = Timer::GetTimeSinceEpochNs();

            LogEntryBuilder builder(s_lineBuffer);
            builder.WriteLine("[%llu][%s][pid=%i][tid=%lu][%s | %s() | %i]",
                timestampNs,
                LogLevelToString(level),
                s_PID,
                threadId,
                fileName,
                function,
                lineNumber);

            builder.WriteLine(format, std::forward<Args>(args)...);
            builder.EndLine();

            for (auto& sink : s_sinks) {
                sink->Write(s_lineBuffer.data(), builder.GetSize());
            }
        }

        static void Flush() noexcept {
            std::lock_guard<SpinLock> lock(s_lock);

            for (auto& sink : s_sinks) {
                sink->Flush();
            }
        }

    private:
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        static inline SpinLock s_lock;
        static inline LogLevel s_maxLevel{ LogLevel::Debug };
        static inline std::vector<char> s_lineBuffer;
        static inline std::vector<std::unique_ptr<ILoggerSink>> s_sinks;
        static inline int s_PID{0};
    };

#define AGT_LOG(level, format, ...) AGT::Logger::Write(level, __FILE__, __func__, __LINE__, format, __VA_ARGS__)
    
}