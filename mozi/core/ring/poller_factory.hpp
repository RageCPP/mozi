#pragma once
#include "mozi/core/ring/event_poller.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "type_traits"
#include <memory>

namespace mozi::ring
{

template <typename I, typename Poller> class mo_poller_factory_c
{
    using sequence_ref = const mo_sequence_t &;

    template <typename U, typename = void, typename... Sequences>
    struct mo_event_poller_t_create_poller_gatting_sequences : std::false_type
    {
    };
    template <typename U, typename... Sequences>
    struct mo_event_poller_t_create_poller_gatting_sequences<
        U,
        std::enable_if_t<std::is_same_v<decltype(std::declval<U>().create_poller(std::declval<const Sequences &>()...)),
                                        std::unique_ptr<Poller>> &&
                         (... && std::is_same_v<Sequences, mo_sequence_t>)>,
        Sequences...> : std::true_type
    {
    };

  public:
    mo_poller_factory_c()
    {
        static_assert(
            std::disjunction<
                mo_event_poller_t_create_poller_gatting_sequences<I>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref, sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref, sequence_ref,
                                                                  sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref, sequence_ref,
                                                                  sequence_ref, sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref, sequence_ref,
                                                                  sequence_ref, sequence_ref, sequence_ref>,
                mo_event_poller_t_create_poller_gatting_sequences<I, sequence_ref, sequence_ref, sequence_ref,
                                                                  sequence_ref, sequence_ref, sequence_ref,
                                                                  sequence_ref>>::value,
            "I should have `std::unique_ptr<mo_event_poller_t<Poller, Event>> create_poller(const "
            "sequence_ref&...)` method");
    }
};
template <typename I, typename Poller> using mo_poller_factory_t = mo_poller_factory_c<I, Poller>;
} // namespace mozi::ring