#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
namespace mozi::ring
{
template <class S> class mo_sequenced_c
{
    template <typename T, typename = void> struct u16_buffer_size : std::false_type
    {
    };
    template <typename T>
    struct u16_buffer_size<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().buffer_size()), uint16_t>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct bool_has_available_capacity_required_capacity : std::false_type
    {
    };
    template <typename T>
    struct bool_has_available_capacity_required_capacity<
        T, std::enable_if_t<
               std::is_same_v<decltype(std::declval<T>().has_available_capacity(std::declval<uint16_t>())), bool>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct size_t_remaining_capacity : std::false_type
    {
    };
    template <typename T>
    struct size_t_remaining_capacity<
        T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().remaining_capacity()), size_t>>> : std::true_type
    {
    };

    template <typename T, typename = void> struct size_t_next : std::false_type
    {
    };
    template <typename T>
    struct size_t_next<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().next()), size_t>>> : std::true_type
    {
    };

    template <typename T, typename = void> struct size_t_next_n : std::false_type
    {
    };
    template <typename T>
    struct size_t_next_n<
        T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().next(std::declval<uint16_t>())), size_t>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct size_t_try_next : std::false_type
    {
    };
    template <typename T>
    struct size_t_try_next<T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().try_next()), size_t>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct size_t_try_next_n : std::false_type
    {
    };
    template <typename T>
    struct size_t_try_next_n<
        T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().try_next(std::declval<uint16_t>())), size_t>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct void_publish_sequence : std::false_type
    {
    };
    template <typename T>
    struct void_publish_sequence<
        T, std::enable_if_t<std::is_same_v<decltype(std::declval<T>().publish(std::declval<size_t>())), void>>>
        : std::true_type
    {
    };

    template <typename T, typename = void> struct void_publish_lo_hi : std::false_type
    {
    };
    template <typename T>
    struct void_publish_lo_hi<
        T, std::enable_if_t<std::is_same_v<
               decltype(std::declval<T>().publish(std::declval<size_t>(), std::declval<size_t>())), void>>>
        : std::true_type
    {
    };

  public:
    mo_sequenced_c()
    {
        static_assert(u16_buffer_size<S>::value, "S should have `uint16_t buffer_size()` method");
        static_assert(bool_has_available_capacity_required_capacity<S>::value,
                      "S should have `bool has_available_capacity(uint16_t required_capacity)` method");
        static_assert(size_t_remaining_capacity<S>::value, "S should have `size_t remaining_capacity()` method");
        static_assert(size_t_next<S>::value, "S should have `size_t next()` method");
        static_assert(size_t_next_n<S>::value, "S should have `size_t next(uint16_t n)` method");
        static_assert(size_t_try_next<S>::value, "S should have `size_t try_next()` method");
        static_assert(size_t_try_next_n<S>::value, "S should have `size_t try_next(uint16_t n)` method");
        static_assert(void_publish_sequence<S>::value, "S should have `void publish(size_t sequence)` method");
        static_assert(void_publish_lo_hi<S>::value, "S should have `void publish(size_t lo, size_t hi)` method");
    }
};
template <class S> using mo_sequenced_t = mo_sequenced_c<S>;
} // namespace mozi::ring