#pragma once
#include "mozi/compile/portability_c.hpp"
namespace mozi
{
//  unsafe_default_initialized
//  unsafe_default_initialized_cv
//
//  An object which is explicitly convertible to any default-constructible type
//  and which, upon conversion, yields a default-initialized value of that type.
//
//  https://en.cppreference.com/w/cpp/language/default_initialization
//
//  For fundamental types, a default-initialized instance may have indeterminate
//  value. Reading an indeterminate value is undefined behavior but may offer a
//  performance optimization. When using an indeterminate value as a performance
//  optimization, it is best to be explicit.
//
//  Useful as an escape hatch when enabling warnings or errors:
//  * gcc:
//    * uninitialized
//    * maybe-uninitialized
//  * clang:
//    * uninitialized
//    * conditional-uninitialized
//    * sometimes-uninitialized
//    * uninitialized-const-reference
//  * msvc:
//    * C4701: potentially uninitialized local variable used
//    * C4703: potentially uninitialized local pointer variable used
//
//  Example:
//
//      int local = folly::unsafe_default_initialized;
//      store_value_into_int_ptr(&value); // suppresses possible warning
//      use_value(value); // suppresses possible warning
struct unsafe_default_initialized_cv
{
    MO_PUSH_WARNING
    // MSVC requires warning disables to be outside of function definition
    // Uninitialized local variable 'uninit' used
    MO_MSVC_DISABLE_WARNING(4700)
    // Potentially uninitialized local variable 'uninit' used
    MO_MSVC_DISABLE_WARNING(4701)
    // Potentially uninitialized local pointer variable 'uninit' used
    MO_MSVC_DISABLE_WARNING(4703)
    MO_GNU_DISABLE_WARNING("-Wuninitialized")
    // Clang doesn't implement -Wmaybe-uninitialized and warns about it
    MO_GCC_DISABLE_WARNING("-Wmaybe-uninitialized")
    template <typename T> MO_ERASE constexpr /* implicit */ operator T() const noexcept
    {
#if defined(__cpp_lib_is_constant_evaluated)
#if __cpp_lib_is_constant_evaluated >= 201811L
#if !defined(__MSVC_RUNTIME_CHECKS)
        if (!std::is_constant_evaluated())
        {
            T uninit;
            return uninit;
        }
#endif // !defined(__MSVC_RUNTIME_CHECKS)
#endif
#endif
        return T();
    }
    MO_POP_WARNING
};
inline constexpr unsafe_default_initialized_cv unsafe_default_initialized{};
} // namespace mozi
