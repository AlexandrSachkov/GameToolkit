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

#if !defined(AGT_ERR) && !defined(AGT_WARN) && !defined(AGT_INFO) && !defined(AGT_VERBOSE) && !defined(AGT_DEBUG)

#ifdef AGT_ENABLE_LOGGING

#include "DefaultLogger.h"
#include "Log.h"

#define AGT_ERR(format, ...)        AGT_LOG_T(AGT::DefaultLogger, AGT::LogLevel::Error, format, __VA_ARGS__)
#define AGT_WARN(format, ...)       AGT_LOG_T(AGT::DefaultLogger, AGT::LogLevel::Warning, format, __VA_ARGS__)
#define AGT_INFO(format, ...)       AGT_LOG_T(AGT::DefaultLogger, AGT::LogLevel::InfoV1, format, __VA_ARGS__)
#define AGT_VERBOSE(format, ...)    AGT_LOG_T(AGT::DefaultLogger, AGT::LogLevel::InfoV3, format, __VA_ARGS__)

#ifdef AGT_ENABLE_DEBUG_LOG
#define AGT_DEBUG(format, ...)      AGT_LOG_T(AGT::DefaultLogger, AGT::LogLevel::Debug, format, __VA_ARGS__)

#else
#define AGT_DEBUG(format, ...) 

#endif //AGT_ENABLE_DEBUG_LOG

#endif //AGT_ENABLE_LOGGING



#endif //!defined(AGT_ERR) && !defined(AGT_WARN) && !defined(AGT_INFO) && !defined(AGT_VERBOSE) && !defined(AGT_DEBUG)