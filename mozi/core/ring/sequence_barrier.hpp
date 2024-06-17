#pragma once
#include <cstddef>
#include <type_traits>

namespace mozi::ring
{
template <class I> class mo_sequence_barrier_c
{
    template <typename U, typename = void> struct size_t_wait_for_sequence : std::false_type
    {
    };
    template <typename U>
    struct size_t_wait_for_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().wait_for(std::declval<size_t>())), size_t>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct size_t_cursor : std::false_type
    {
    };
    template <typename U>
    struct size_t_cursor<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().cursor()), size_t>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_is_alerted : std::false_type
    {
    };
    template <typename U>
    struct bool_is_alerted<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().is_alerted()), bool>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct void_alert : std::false_type
    {
    };
    template <typename U>
    struct void_alert<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().alert()), void>>> : std::true_type
    {
    };

    template <typename U, typename = void> struct void_clear_alert : std::false_type
    {
    };
    template <typename U>
    struct void_clear_alert<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().clear_alert()), void>>>
        : std::true_type
    {
    };

  public:
    mo_sequence_barrier_c()
    {
        static_assert(size_t_wait_for_sequence<I>::value, "I should have `size_t wait_for(size_t)` method");
        static_assert(size_t_cursor<I>::value, "I should have `size_t cursor()` method");
        static_assert(bool_is_alerted<I>::value, "I should have `bool is_alerted()` method");
        static_assert(void_alert<I>::value, "I should have `void alert()` method");
        static_assert(void_clear_alert<I>::value, "I should have `void clear_alert()` method");
    }
};
template <class I> using mo_sequence_barrier_t = mo_sequence_barrier_c<I>;
} // namespace mozi::ring