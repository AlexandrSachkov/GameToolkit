#pragma once

#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
#define AGT_COMPILER_GCC

#elif defined(__INTEL_COMPILER)
#define AGT_COMPILER_INTEL

#elif defined(__clang__)
#define AGT_COMPILER_CLANG

#elif defined(_MSC_VER)
#define AGT_COMPILER_MSVC

#endif