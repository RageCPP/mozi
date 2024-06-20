#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "type_traits"
#include <memory>
// clang-format off
namespace mozi::ring
{
template <typename I, typename SequenceBarrier> class mo_sequence_barrier_factory_c
{
    template <typename U, typename = void, typename... Sequences>
    struct mo_sequence_barrier_t_create_barrier_sequences_to_track : std::false_type
    {
    };
    template <typename U, typename... Sequences>
    struct mo_sequence_barrier_t_create_barrier_sequences_to_track<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().create_barrier(std::declval<Sequences &>()...)), std::unique_ptr<SequenceBarrier>> 
            && (... && std::is_same_v<Sequences, mo_arc_sequence_t>)>,
        Sequences...
    > : std::true_type
    {
    };

  public:
    mo_sequence_barrier_factory_c()
    {
        static_assert(
            std::disjunction<
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>,
                mo_sequence_barrier_t_create_barrier_sequences_to_track<I, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &, mo_arc_sequence_t &>>::value,
            "I should have `std::unique_ptr<mo_sequence_barrier_t<SequenceBarrier>> create_barrier(mo_arc_sequence_t&...)` method");
    }
};
template <typename I, typename SequenceBarrier>
using mo_sequence_barrier_factory_t = mo_sequence_barrier_factory_c<I, SequenceBarrier>;
} // namespace mozi::ring
// clang-format on
