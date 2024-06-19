#pragma once
#include "mozi/core/ring/event_translator.hpp"
#include "type_traits"

namespace mozi::ring
{
/*! \typedef Event The event type
 */
template <class I, typename Event, typename Translator>
class mo_event_sink_c : public mo_event_translator_t<Translator, Event>
{
    template <typename U, typename = void> struct void_publish_event_translator : std::false_type
    {
    };
    template <typename U>
    struct void_publish_event_translator<
        U, std::enable_if_t<
               std::is_same_v<decltype(std::declval<U>().publish_event(std::declval<const Translator &>())), void>>>
        : std::true_type
    {
    };

    // TODO: 改为 使用元组的方式

    // template <typename U, typename = void> struct void_publish_events_translators : std::false_type
    // {
    // };
    // template <typename U>
    // struct void_publish_events_translators<
    //     U, std::enable_if_t<std::is_same_v<
    //            decltype(std::declval<U>().publish_events(std::declval<const std::vector<Event> &>())), void>>>
    //     : std::true_type
    // {
    // };

    // template <typename U, typename = void> struct bool_try_publish_events_translator : std::false_type
    // {
    // };
    // template <typename U>
    // struct bool_try_publish_events_translator<
    //     U, std::enable_if_t<std::is_same_v<
    //            decltype(std::declval<U>().try_publish_events(std::declval<const std::vector<Event> &>())), bool>>>
    //     : std::true_type
    // {
    // };

  public:
    mo_event_sink_c()
    {
        static_assert(void_publish_event_translator<I>::value,
                      "I must have `void publish_event(const mo_event_translator_t<I, Event> &)` method");
        // static_assert(void_publish_events_translators<I>::value,
        //               "I must have `void publish_events(const std::vector<Event> &)` method");
        // static_assert(bool_try_publish_events_translator<I>::value,
        //               "I must have `bool try_publish_events(const std::vector<Event> &)` method");
    }
};
template <class I, typename Event, typename Translator> using mo_event_sink_t = mo_event_sink_c<I, Event, Translator>;
} // namespace mozi::ring