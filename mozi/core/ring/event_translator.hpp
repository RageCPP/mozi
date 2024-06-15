#pragma once
#include <cstddef>
#include <type_traits>
namespace mozi::ring
{
/*!
  \typedef typename I instance of the event translator
  \typedef typename Event event implementation storing the data for sharing during exchange or parallel coordination of
  an event.
 */
template <typename I, typename Event> class mo_event_translator_c
{
    template <typename U, typename = void> struct void_translate_to_event_sequence : std::false_type
    {
    };
    template <typename U>
    struct void_translate_to_event_sequence<
        U, std::enable_if_t<std::is_same_v<
               decltype(std::declval<U>().translate_to(std::declval<Event>(), std::declval<size_t>())), void>>>
        : std::true_type
    {
    };

  public:
    mo_event_translator_c()
    {
        static_assert(void_translate_to_event_sequence<I>::value,
                      "I must have `void translate_to(Event, size_t)` method");
    }
};
template <typename I, typename Event> using mo_event_translator_t = mo_event_translator_c<I, Event>;
} // namespace mozi::ring