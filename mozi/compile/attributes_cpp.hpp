#pragma once
#include "portability.hpp"

#if MO_HAS_CPP_ATTRIBUTE(gnu::cold)
#define MO_COLD gnu::cold
#else
#define MO_COLD
#endif

#if MO_HAS_CPP_ATTRIBUTE(gnu::unused)
#define MO_UNUSED gnu::unused
#else
#define MO_UNUSED
#endif

#if MO_HAS_CPP_ATTRIBUTE(nodiscard)
#define MO_NODISCARD nodiscard
#else
#define MO_NODISCARD
#endif

// 增加低于cpp 20的编译器的支持
// 使用 [[likely]] 和 [[unlikely]] 会导致低于cpp 20的编译器报错
// 使用 __builtin_expect 替代 folly\lang\Builtin.h folly\Likely.h
#if MO_HAS_CPP_ATTRIBUTE(likely)
#define MO_LIKELY likely
#else
#define MO_LIKELY
#endif

#if MO_HAS_CPP_ATTRIBUTE(unlikely)
#define MO_UNLIKELY unlikely
#else
#define MO_UNLIKELY
#endif
