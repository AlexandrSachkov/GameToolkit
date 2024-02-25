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

#include <filesystem>
#include <fstream>
#include <vector>

namespace AGT {
    class LoggerFileSink : public ILoggerSink {
    public:
        LoggerFileSink() = default;

        ~LoggerFileSink() {
            if (m_file.is_open()) {
                m_file.close();
            }
        }

        bool Init(const std::filesystem::path& path, size_t buffSize = 0) {
            if (buffSize > 0) {
                m_buffer.resize(buffSize);
                m_file.rdbuf()->pubsetbuf(&m_buffer.front(), m_buffer.size());
            }

            m_file.open(path.c_str(), std::ios::trunc);
            return m_file.is_open();
        }

        void Write(const char* msg, size_t size) override {
            if (!m_file.is_open()) {
                return;
            }

            m_file.write(msg, size);
        }

        void Flush() override {
            m_file.flush();
        }
    private:
        LoggerFileSink(const LoggerFileSink&) = delete;
        LoggerFileSink& operator=(const LoggerFileSink&) = delete;

        std::ofstream m_file;
        std::vector<char> m_buffer;
    };
}