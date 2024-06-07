#pragma once
#include <functional>
#include <type_traits>
namespace mozi
{
struct Ignore
{
    Ignore() = default;
    template <class T> constexpr /* implicit */ Ignore(const T &)
    {
    }
    template <class T> const Ignore &operator=(T const &) const
    {
        return *this;
    }
};

template <class...> using Ignored = Ignore;

namespace traits_detail_IsEqualityComparable
{
Ignore operator==(Ignore, Ignore);

template <class T, class U = T>
struct IsEqualityComparable : std::is_convertible<decltype(std::declval<T>() == std::declval<U>()), bool>
{
};
} // namespace traits_detail_IsEqualityComparable

/* using override */ using traits_detail_IsEqualityComparable::IsEqualityComparable;

namespace traits_detail_IsLessThanComparable
{
Ignore operator<(Ignore, Ignore);

template <class T, class U = T>
struct IsLessThanComparable : std::is_convertible<decltype(std::declval<T>() < std::declval<U>()), bool>
{
};
} // namespace traits_detail_IsLessThanComparable

/* using override */ using traits_detail_IsLessThanComparable::IsLessThanComparable;

template <bool... Bs> struct Bools
{
    using valid_type = bool;
    static constexpr std::size_t size()
    {
        return sizeof...(Bs);
    }
};

// Lighter-weight than Conjunction, but evaluates all sub-conditions eagerly.
template <class... Ts> struct StrictConjunction : std::is_same<Bools<Ts::value...>, Bools<(Ts::value || true)...>>
{
};

/**
 * A type trait to remove all const volatile and reference qualifiers on a
 * type T
 */
template <typename T> struct remove_cvref
{
    using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};
template <typename T> using remove_cvref_t = typename remove_cvref<T>::type;

namespace detail
{
template <bool> struct conditional_;
template <> struct conditional_<false>
{
    template <typename, typename T> using apply = T;
};
template <> struct conditional_<true>
{
    template <typename T, typename> using apply = T;
};
} // namespace detail

//  conditional_t
//
//  Like std::conditional_t but with only two total class template instances,
//  rather than as many class template instances as there are uses.
//
//  As one effect, the result can be used in deducible contexts, allowing
//  deduction of conditional_t<V, T, F> to work when T or F is a template param.
template <bool V, typename T, typename F> using conditional_t = typename detail::conditional_<V>::template apply<T, F>;

template <typename ReturnType, typename... Args> using mo_function_t = std::function<ReturnType(Args...)>;
} // namespace mozi
