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
#include "../log/DefaultLoggerConfig.h"

#include <atomic>
#include <exception>
#include <functional>
#include <signal.h>

#ifdef AGT_PLAT_WINDOWS
#include <Windows.h>
#include <Dbghelp.h>
#endif

namespace AGT {
    enum class MinidumpSize {
        None,
        Small,
        Medium,
        Large
    };

    class CrashHandler {
    public:
        static void Init(MinidumpSize minidumpSize, const char* crashDumpPath, const std::function<void()>& fOnTerminate) noexcept {
            m_minidumpSize = minidumpSize;
            m_fOnTerminate = fOnTerminate;
            m_crashDumpPath = crashDumpPath;

            SetUnhandledExceptionFilter(::AGT::CrashHandler::UnhandledExceptionHandler);
            signal(SIGABRT, &::AGT::CrashHandler::AbortHandler);
        }

    private:
        static void AbortHandler(int) {
            AGT_ERR("Abort called. Terminating program.");

#ifdef AGT_PLAT_WINDOWS
            __try {
                int* createException = nullptr;
                *createException = 1;
            } __except (UnhandledExceptionHandler(GetExceptionInformation())) {}
#else
            //TODO is it possibe to get the current exception
            m_fOnTerminate();
#endif
        }

        static LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS* ex) noexcept {
            AGT_ERR("Unhanded exception was thrown. Terminating program.");

            if (m_minidumpSize != MinidumpSize::None && m_crashDumpPath && !m_minidumpGenerated) {
#ifdef AGT_PLAT_WINDOWS
                SaveMinidump(ex, m_crashDumpPath, m_minidumpSize);
#else
                //TODO add platform support
#endif
            }
            
            m_fOnTerminate();
            return EXCEPTION_CONTINUE_SEARCH;
        }

#ifdef AGT_PLAT_WINDOWS
        static void SaveMinidump(EXCEPTION_POINTERS* ex, const char* crashDumpPath, MinidumpSize size) noexcept {
            AGT_INFO("Attempting to generate a minidump.");

            static const char* LIB_NAME = "dbghelp";

            auto hDbgHelp = LoadLibraryA(LIB_NAME);
            if (hDbgHelp == nullptr) {
                AGT_ERR("Failed to load library %s: GetLastError=%i", LIB_NAME, GetLastError());
                return;
            }

            static const char* FUNC_NAME = "MiniDumpWriteDump";
            auto fMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, FUNC_NAME);
            if (!fMiniDumpWriteDump) {
                AGT_ERR("GetProcAddress failed %s: GetLastError=%i", FUNC_NAME, GetLastError());
                return;
            }

            auto hFile = CreateFileA(crashDumpPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
            if (hFile == INVALID_HANDLE_VALUE) {
                AGT_ERR("CreateFileA failed for path %s: GetLastError=%i", crashDumpPath, GetLastError());
                return;
            }

            MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
            exceptionInfo.ThreadId = GetCurrentThreadId();
            exceptionInfo.ExceptionPointers = ex;
            exceptionInfo.ClientPointers = FALSE;

            int type = MiniDumpNormal;
            if (size == MinidumpSize::Large) {
                type |= MiniDumpWithDataSegs
                    | MiniDumpWithPrivateReadWriteMemory
                    | MiniDumpWithHandleData
                    | MiniDumpWithFullMemory
                    | MiniDumpWithFullMemoryInfo
                    | MiniDumpWithThreadInfo
                    | MiniDumpWithUnloadedModules
                    | MiniDumpWithProcessThreadData;
            } else if (size == MinidumpSize::Medium) {
                type |= MiniDumpWithDataSegs
                    | MiniDumpWithPrivateReadWriteMemory
                    | MiniDumpWithHandleData
                    | MiniDumpWithFullMemoryInfo
                    | MiniDumpWithThreadInfo
                    | MiniDumpWithUnloadedModules;
            } else {
                type |= MiniDumpWithIndirectlyReferencedMemory
                    | MiniDumpScanMemory;
            }

            auto dumped = fMiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                (MINIDUMP_TYPE)type,
                ex ? &exceptionInfo : nullptr,
                nullptr,
                nullptr);

            if (!dumped) {
                AGT_ERR("Failed to write crash dump: GetLastError=%i", GetLastError());
            } else {
                AGT_INFO("Minidump generated successfully");
                m_minidumpGenerated = true;
            }

            CloseHandle(hFile);
        }
#endif
        static inline MinidumpSize m_minidumpSize{ MinidumpSize::Small };
        static inline const char* m_crashDumpPath{ nullptr };
        static inline std::function<void()> m_fOnTerminate;
        static inline std::atomic_bool m_minidumpGenerated{ false };
    };
    
}