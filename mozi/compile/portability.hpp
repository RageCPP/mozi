#pragma once

#ifndef __has_cpp_attribute
#define MO_HAS_CPP_ATTRIBUTE(x) 0
#else
#define MO_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif

namespace mozi
{
#ifdef NDEBUG
constexpr bool mo_is_debug_const = false;
#else
constexpr bool mo_is_debug_const = true;
#endif
} // namespace mozi
