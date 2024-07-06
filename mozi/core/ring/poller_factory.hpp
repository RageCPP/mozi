#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "type_traits"
#include <memory>

// clang-format off
namespace mozi::ring
{

template <typename I, typename Poller> class mo_poller_factory_c
{

    template <typename U, typename = void, typename... Sequences>
    struct mo_event_poller_t_create_poller_gatting_sequences : std::false_type
    {
    };
    template <typename U, typename... Sequences>
    struct mo_event_poller_t_create_poller_gatting_sequences<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().create_poller(std::declval<Sequences>()...)), std::unique_ptr<Poller>>
            && (... && std::is_same_v<Sequences, mo_arc_sequence_t>)>,
        Sequences...
    > : std::true_type
    {
    };

  public:
    mo_poller_factory_c()
    {
        static_assert(
            std::disjunction<
                mo_event_poller_t_create_poller_gatting_sequences<I>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_event_poller_t_create_poller_gatting_sequences<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>>::value,
            "I should have `std::unique_ptr<mo_event_poller_t<Poller, Event>> create_poller(mo_arc_sequence_t& ...)` method");
    }
};
template <typename I, typename Poller> using mo_poller_factory_t = mo_poller_factory_c<I, Poller>;
} // namespace mozi::ring
// clang-format on