#pragma once
#include <cstddef>
#include <type_traits>
namespace mozi::disruptor
{
template <class T> class mo_cursored_c
{
    template <typename U, typename = void> struct size_t_cursor : std::false_type
    {
    };
    template <typename U>
    struct size_t_cursor<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().cursor()), size_t>>>
        : std::true_type
    {
    };

  public:
    mo_cursored_c()
    {
        static_assert(size_t_cursor<T>::value, "T should have `size_t cursor()` method");
    }
};
template <class T> using mo_cursored_t = mo_cursored_c<T>;
} // namespace mozi::disruptor