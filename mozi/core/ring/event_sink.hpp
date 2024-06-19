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
    template <typename U, typename = void, typename... Translators>
    struct bool_publish_events_translators : std::false_type
    {
    };
    template <typename U, typename... Translators>
    struct bool_publish_events_translators<
        U,
        std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().publish_events(std::declval<const Translators &>()...)), bool>>,
        Translators...> : std::true_type
    {
    };

  public:
    mo_event_sink_c()
    {
        using translator_ref = const Translator &;
        static_assert(void_publish_event_translator<I>::value,
                      "I must have `void publish_event(const mo_event_translator_t<I, Event> &)` method");

        // clang-format off
        static_assert(
            std::disjunction<
                bool_publish_events_translators<I>,
                bool_publish_events_translators<I, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>,
                bool_publish_events_translators<I, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref, translator_ref>
            >::value,
            "I must have `bool publish_events(const Translators &...)` method");
        // clang-format on
    }
};
template <class I, typename Event, typename Translator> using mo_event_sink_t = mo_event_sink_c<I, Event, Translator>;
} // namespace mozi::ring