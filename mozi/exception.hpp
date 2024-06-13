#pragma once

#include "compile/attributes_cpp.hpp"
#include "compile/portability_c.hpp"
#include "traits.hpp"
#include <fmt/format.h>
namespace mozi
{
/// throw_exception
///
/// Throw an exception if exceptions are enabled, or terminate if compiled with
/// -fno-exceptions.
template <typename Ex> [[noreturn, MO_COLD]] MO_NOINLINE void throw_exception(Ex &&ex)
{
#if MO_HAS_EXCEPTIONS
    throw static_cast<Ex &&>(ex);
#else
    (void)ex;
    std::terminate();
#endif
}

namespace detail
{

struct throw_exception_arg_array_
{
    template <typename R> using v = std::remove_extent_t<std::remove_reference_t<R>>;
    template <typename R> using apply = std::enable_if_t<std::is_same<char const, v<R>>::value, v<R> *>;
};
struct throw_exception_arg_trivial_
{
    template <typename R> using apply = remove_cvref_t<R>;
};
struct throw_exception_arg_base_
{
    template <typename R> using apply = R;
};
template <typename R>
using throw_exception_arg_ = //
    conditional_t<std::is_array<std::remove_reference_t<R>>::value, throw_exception_arg_array_,
                  conditional_t<std::is_trivially_copyable_v<remove_cvref_t<R>>, throw_exception_arg_trivial_,
                                throw_exception_arg_base_>>;
template <typename R> using throw_exception_arg_t = typename throw_exception_arg_<R>::template apply<R>;

template <typename Ex, typename... Args> [[noreturn, MO_COLD]] MO_NOINLINE void throw_exception_(Args... args)
{
    throw_exception(Ex(static_cast<Args>(args)...));
}
template <typename Ex, typename... Args> [[noreturn, MO_COLD]] MO_NOINLINE void terminate_with_(Args... args) noexcept
{
    throw_exception(Ex(static_cast<Args>(args)...));
}

} // namespace detail

/// throw_exception
///
/// Construct and throw an exception if exceptions are enabled, or terminate if
/// compiled with -fno-exceptions.
///
/// Does not perfectly forward all its arguments. Instead, in the interest of
/// minimizing common-case inline code size, decays its arguments as follows:
/// * refs to arrays of char const are decayed to char const*
/// * refs to arrays are otherwise invalid
/// * refs to trivial types are decayed to values
///
/// The reason for treating refs to arrays as invalid is to avoid having two
/// behaviors for refs to arrays, one for the general case and one for where the
/// inner type is char const. Having two behaviors can be surprising, so avoid.
template <typename Ex, typename... Args> [[noreturn]] MO_ERASE void throw_exception(Args &&...args)
{
    detail::throw_exception_<Ex, detail::throw_exception_arg_t<Args &&>...>(static_cast<Args &&>(args)...);
}
} // namespace mozi
