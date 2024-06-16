#pragma once
#include <cstddef>
#include <type_traits>
namespace mozi::ring
{
template <class SI, typename Event> class mo_data_provider_c
{
    template <class U, typename = void> struct event_get_sequence : std::false_type
    {
    };
    template <class U>
    struct event_get_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>()[std::declval<size_t>()]), Event>>>
        : std::true_type
    {
    };

  public:
    mo_data_provider_c()
    {
        static_assert(event_get_sequence<SI>::value, "SI should have `Event operator[](size_t)` method");
    }
};
template <class SI, typename Event> using mo_data_provider_t = mo_data_provider_c<SI, Event>;
} // namespace mozi::ring