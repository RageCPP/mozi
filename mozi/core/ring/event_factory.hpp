#pragma once

#include <type_traits>
namespace mozi::ring
{
template <typename T> class mo_event_factory_c
{
    template <typename U, typename = void> struct u_create_instance : std::false_type
    {
    };
    template <typename U>
    struct u_create_instance<U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().create_instance()), U>>>
        : std::true_type
    {
    };

  public:
    mo_event_factory_c()
    {
        static_assert(u_create_instance<T>::value, "T must have `T create_instance()` method");
    }
};
template <typename T> using mo_event_factory_t = mo_event_factory_c<T>;
} // namespace mozi::ring