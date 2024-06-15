#pragma once
#include "mozi/core/ring/event_translator.hpp"
#include "type_traits"
#include <vector>

namespace mozi::ring
{
/*! \typedef Event The event type
 */
template <class I, typename Event> class mo_event_sink_c
{
    template <typename U, typename = void> struct void_publish_event_translator : std::false_type
    {
    };
    template <typename U>
    struct void_publish_event_translator<
        U, std::enable_if_t<std::is_same_v<
               decltype(std::declval<U>().publish_event(std::declval<mo_event_translator_t<I, Event>>())), void>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct void_publish_events_translators : std::false_type
    {
    };
    template <typename U>
    struct void_publish_events_translators<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().publish_events(
                                               std::declval<std::vector<mo_event_translator_t<I, Event>>>())),
                                           void>>> : std::true_type
    {
    };

  public:
    mo_event_sink_c()
    {
        static_assert(void_publish_event_translator<I>::value,
                      "Event must have `void publish_event(mo_event_translator_t<I, Event>)` method");
        static_assert(void_publish_events_translators<I>::value,
                      "Event must have `void publish_events(std::vector<mo_event_translator_t<I, Event>>)` method");
    }
};
template <class I, typename Event> using mo_event_sink_t = mo_event_sink_c<I, Event>;
} // namespace mozi::ring