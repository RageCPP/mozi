#pragma once
#include <cstddef>
#include <type_traits>
namespace mozi::ring
{
template <template <auto> class T> class mo_data_provider_c
{
    template <template <auto> class U, typename = void> struct D_get_sequence : std::false_type
    {
    };
    template <template <auto> class U>
    struct D_get_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U<0>>().get(std::declval<size_t>())), decltype(0)>>>
        : std::true_type
    {
    };

  public:
    mo_data_provider_c()
    {
        static_assert(D_get_sequence<T>::value, "T should have `D get(size_t)` method");
    }
};
template <template <auto> class T> using mo_data_provider_t = mo_data_provider_c<T>;
} // namespace mozi::ring