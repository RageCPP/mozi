#pragma once
#include "mozi/compile/portability_c.hpp"
#include <assert.h>

#define MO_SINGLE_ARG(...) __VA_ARGS__

#ifndef MO_COMPILE_VARIABLE

#define MO_CONCATENATE_IMPL(s0, s1) s0##s1
#define MO_CONCATENATE(s0, s1) MO_CONCATENATE_IMPL(s0, s1)

#ifdef __COUNTER__

#if MO_HAS_FEATURE(modules)
#define MO_COMPILE_VARIABLE(str) MO_CONCATENATE(MO_CONCATENATE(MO_CONCATENATE(str, __COUNTER__), _), __LINE__)
#else
#define MO_COMPILE_VARIABLE(str) MO_CONCATENATE(str, __COUNTER__)
#endif

#else
#define MO_COMPILE_VARIABLE(str) MO_CONCATENATE(str, __LINE__)
#endif

#endif

#ifdef NDEBUG
#define MO_ASSERT(condition, message)
#else
#define MO_ASSERT(condition, message)                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            __assert_fail(#message, __FILE__, __LINE__, __ASSERT_FUNCTION);                                            \
        }                                                                                                              \
    } while (false)
#endif
