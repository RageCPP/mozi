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

    // clang-format off
    template <typename U>
    struct void_translate_to_event_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>()(std::declval<Event &>(), size_t())), void>>> : std::true_type
    {
    };
    // clang-format on

  public:
    mo_event_translator_c()
    {
        // clang-format off
        static_assert(void_translate_to_event_sequence<I>::value, "I must have `void oprator(Event& source_event, size_t source_event_sequence)` method");
        // clang-format on
    }
};
template <typename I, typename Event> using mo_event_translator_t = mo_event_translator_c<I, Event>;
} // namespace mozi::ring