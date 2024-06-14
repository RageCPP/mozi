#pragma once
#include "mozi/compile/portability_c.hpp"

#ifndef __has_cpp_attribute
#define MO_HAS_CPP_ATTRIBUTE(x) 0
#else
#define MO_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif

// Define MO_HAS_EXCEPTIONS
#if (defined(__cpp_exceptions) && __cpp_exceptions >= 199711) || MO_HAS_FEATURE(cxx_exceptions)
#define MO_HAS_EXCEPTIONS 1
#elif __GNUC__
#if defined(__EXCEPTIONS) && __EXCEPTIONS
#define MO_HAS_EXCEPTIONS 1
#else // __EXCEPTIONS
#define MO_HAS_EXCEPTIONS 0
#endif // __EXCEPTIONS
#elif MO_MICROSOFT_ABI_VER
#if _CPPUNWIND
#define MO_HAS_EXCEPTIONS 1
#else // _CPPUNWIND
#define MO_HAS_EXCEPTIONS 0
#endif // _CPPUNWIND
#else
#define MO_HAS_EXCEPTIONS 1 // default assumption for unknown platforms
#endif

namespace mozi
{
#ifdef NDEBUG
constexpr bool mo_is_debug_const = false;
#else
constexpr bool mo_is_debug_const = true;
#endif
} // namespace mozi
