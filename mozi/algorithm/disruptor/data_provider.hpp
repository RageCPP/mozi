#pragma once
#include <cstddef>
#include <type_traits>
namespace mozi::disruptor
{
template <class T, typename D> class mo_data_provider_c
{
    template <typename U, typename = void> struct D_get_sequence : std::false_type
    {
    };
    template <typename U>
    struct D_get_sequence<U,
                          std::enable_if_t<std::is_same_v<decltype(std::declval<U>().get(std::declval<size_t>())), D>>>
        : std::true_type
    {
    };

  public:
    mo_data_provider_c()
    {
        static_assert(D_get_sequence<T>::value, "T should have `D get(size_t)` method");
    }
};
template <class T, typename D> using mo_data_provider_t = mo_data_provider_c<T, D>;
} // namespace mozi::disruptor