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

#include "ILogger.h"
#include "LogLevel.h"

#include <memory>

namespace AGT {
    template<typename T>
    class Log {
    public:
        static void SetLogger(const std::shared_ptr<ILogger<T>>& logger) {
            m_logger = logger;
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
            if (m_logger) {
                m_logger->Write(level, file, function, lineNumber, format, std::forward<Args>(args)...);
            }
        }

        static void Flush() noexcept {
            if (m_logger) {
                m_logger->Flush();
            }
        }

    private:
        Log(const Log&) = delete;
        Log& operator=(const Log&) = delete;

        static inline std::shared_ptr<ILogger<T>> m_logger;
    };
}

#define AGT_LOG_T(T, level, format, ...) AGT::Log<T>::Write(level, __FILE__, __func__, __LINE__, format, __VA_ARGS__)