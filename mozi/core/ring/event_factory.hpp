#pragma once

#include <type_traits>
namespace mozi::ring
{
template <typename T, typename Event> class mo_event_factory_c
{
    template <typename U, typename = void> struct event_create_instance : std::false_type
    {
    };
    template <typename U>
    struct event_create_instance<U, std::enable_if_t<std::is_same_v<decltype(U::create_instance()), Event>>>
        : std::true_type
    {
    };

  public:
    mo_event_factory_c()
    {
        static_assert(event_create_instance<T>::value, "T must have `static Event create_instance()` method");
    }
};
template <typename T, typename Event> using mo_event_factory_t = mo_event_factory_c<T, Event>;
} // namespace mozi::ring