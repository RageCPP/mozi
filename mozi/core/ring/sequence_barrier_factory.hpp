#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "type_traits"
#include <memory>

namespace mozi::ring
{
template <typename I, typename SequenceBarrier> class mo_sequence_barrier_factory_c
{
    using sequence_ref = const mo_sequence_t &;

    // clang-format off
    template <typename U, typename = void, typename... Sequences>
    struct mo_sequence_barrier_t_create_barrier_sequences_to_track : std::false_type
    {
    };
    template <typename U, typename... Sequences>
    struct mo_sequence_barrier_t_create_barrier_sequences_to_track<
        U,
        std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().create_barrier(std::declval<const Sequences &>()...)), std::unique_ptr<SequenceBarrier>> &&
            (... && std::is_same_v<Sequences, mo_sequence_t>)>,
        Sequences...> : std::true_type
    {
    };
    // clang-format on

  public:
    mo_sequence_barrier_factory_c()
    {
        static_assert(
            std::disjunction<
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref, sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref, sequence_ref,
                                                                        sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref, sequence_ref,
                                                                        sequence_ref, sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref, sequence_ref,
                                                                        sequence_ref, sequence_ref, sequence_ref>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, sequence_ref, sequence_ref, sequence_ref,
                                                                        sequence_ref, sequence_ref, sequence_ref,
                                                                        sequence_ref>>::value,
            "I should have `std::unique_ptr<mo_sequence_barrier_t<SequenceBarrier>> create_barrier(const "
            "sequence_ref&...)` method");
    }
};
template <typename I, typename SequenceBarrier>
using mo_sequence_barrier_factory_t = mo_sequence_barrier_factory_c<I, SequenceBarrier>;
} // namespace mozi::ring