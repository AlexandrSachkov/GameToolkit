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

//See https://sourceforge.net/p/predef/wiki/OperatingSystems/

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define AGT_PLAT_WINDOWS

#elif defined(unix) || defined(__unix) || defined(__unix__)
#define AGT_PLAT_UNIX

#elif defined(__linux__) || defined(linux) || defined(__linux)
#define AGT_PLAT_LINUX

#elif defined(__APPLE__) || defined(__MACH__)
#define AGT_PLAT_MACOS

#elif defined(__FreeBSD__)
#define AGT_PLAT_FREEBSD

#elif defined(__ANDROID__)
#define AGT_PLAT_ANDROID

#endif
