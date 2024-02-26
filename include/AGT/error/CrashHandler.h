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

#include <functional>
#include <signal.h>

#ifdef AGT_PLAT_WINDOWS
#include <Windows.h>
#include <Dbghelp.h>
#endif

namespace AGT {
    class CrashHandler {
    public:
        static bool Init(const char* crashDumpPath, const std::function<void()>& fOnTerminate) {
            m_fOnTerminate = fOnTerminate;
            m_crashDumpPath = crashDumpPath;

            SetUnhandledExceptionFilter(::AGT::CrashHandler::UnhandledExceptionHandler);
            std::set_terminate(::AGT::CrashHandler::TerminateHandler);
            signal(SIGABRT, &::AGT::CrashHandler::AbortHandler);

            return true;
        }

    private:
        static void AbortHandler(int) {
            AGT_ERR("Abort called. Terminating program.");
            m_fOnTerminate();
        }

        static LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS* ex) {
            AGT_ERR("Unhanded exception was thrown. Terminating program.");

            if (m_crashDumpPath) {
#ifdef AGT_PLAT_WINDOWS
                SaveMinidump(ex, m_crashDumpPath);
#else
                //TODO add platform support
#endif
            }
            
            m_fOnTerminate();
            return EXCEPTION_CONTINUE_SEARCH;
        }

        static void TerminateHandler() {
            AGT_ERR("Terminate called. Terminating program.");
            m_fOnTerminate();
        }

#ifdef AGT_PLAT_WINDOWS
        static void SaveMinidump(EXCEPTION_POINTERS* ex, const char* crashDumpPath) {
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

            auto dumped = fMiniDumpWriteDump(
                GetCurrentProcess(),
                GetCurrentProcessId(),
                hFile,
                MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
                ex ? &exceptionInfo : nullptr,
                nullptr,
                nullptr);

            if (!dumped) {
                AGT_ERR("Failed to write crash dump: GetLastError=%i", GetLastError());
            } else {
                AGT_INFO("Minidump generated successfully");
            }

            CloseHandle(hFile);
        }
#endif

        static inline const char* m_crashDumpPath{ nullptr };
        static inline std::function<void()> m_fOnTerminate;
    };
    
}