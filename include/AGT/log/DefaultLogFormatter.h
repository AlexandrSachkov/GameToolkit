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

#include "../platform/Platform.h"
#include "LogEntryBuilder.h"
#include "LogLevel.h"

namespace AGT {
    class DefaultLogFormatter {
    public:
        DefaultLogFormatter() noexcept = default;

        template<typename... Args>
        void Format(
            LogEntryBuilder& builder,
            LogLevel level,
            const char* file,
            const char* function,
            int lineNumber,
            const char* format,
            Args&&... args
        ) noexcept {
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

            int pid = _getpid();
            size_t threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());

            auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
            uint64_t timestampNs = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();

            builder.WriteLine("[%llu][%s][pid=%i][tid=%llu][%s | %s() | %i]",
                              timestampNs,
                              LogLevelToString(level),
                              pid,
                              threadId,
                              fileName,
                              function,
                              lineNumber);

            builder.WriteLine(format, std::forward<Args>(args)...);
            builder.EndLine();
        }
    };
}