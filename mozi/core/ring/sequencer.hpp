#pragma once
#include "mozi/core/ring/cursored.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequenced.hpp"
#include <cstddef>
#include <type_traits>
// clang-format off
namespace mozi::ring
{
template <class Sequencer, typename Event> class mo_sequencer_c : public mo_cursored_t<Sequencer>, public mo_sequenced_t<Sequencer>
{
    template <typename U, typename = void> struct void_claim_sequence : std::false_type
    {
    };
    template <typename U>
    struct void_claim_sequence<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().claim(std::declval<size_t>())), void>>
    > : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_is_available_sequence : std::false_type
    {
    };
    template <typename U>
    struct bool_is_available_sequence<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().is_available(std::declval<size_t>())), bool>>
    > : std::true_type
    {
    };

    template <typename U, typename = void, typename... Sequences>
    struct void_add_gating_sequences_gating_sequences : std::false_type
    {
    };
    template <typename U, typename... Sequences>
    struct void_add_gating_sequences_gating_sequences<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().add_gating_sequences(std::declval<Sequences &>()...)), void> 
            && (... && std::is_same_v<Sequences, mo_arc_sequence_t>)>,
        Sequences...
    > : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_remove_gating_sequences_sequence : std::false_type
    {
    };
    template <typename U>
    struct bool_remove_gating_sequences_sequence<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().remove_gating_sequences(std::declval<mo_arc_sequence_t &>())), bool>>
    > : std::true_type
    {
    };

    template <typename U, typename = void> struct size_t_minimum_sequence : std::false_type
    {
    };
    template <typename U>
    struct size_t_minimum_sequence<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().minimum_sequence()), size_t>>
    > : std::true_type
    {
    };

    template <typename U, typename = void>
    struct size_t_highest_published_sequence_next_sequence_available_sequence : std::false_type
    {
    };
    template <typename U>
    struct size_t_highest_published_sequence_next_sequence_available_sequence<
        U, std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().highest_published_sequence(std::declval<size_t>(), std::declval<size_t>())), size_t>>
    > : std::true_type
    {
    };

  public:
    size_t INITIAL_CURSOR_VALUE = -1;
    mo_sequencer_c()
    {
        static_assert(void_claim_sequence<Sequencer>::value, "Sequencer should have `void claim(size_t)` method");
        static_assert(bool_is_available_sequence<Sequencer>::value, "Sequencer should have `bool is_available(size_t)` method");
        static_assert(
            std::disjunction<
                void_add_gating_sequences_gating_sequences<Sequencer>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>,
                void_add_gating_sequences_gating_sequences<Sequencer, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&, mo_arc_sequence_t&>
            >::value, "Sequencer should have `void add_gating_sequences(mo_arc_sequence_t& ...)` method");
        static_assert(bool_remove_gating_sequences_sequence<Sequencer>::value,  "Sequencer should have `bool remove_gating_sequence(mo_arc_sequence_t&)` method");
        static_assert(size_t_minimum_sequence<Sequencer>::value, "Sequencer should have `size_t minimum_sequence()` method");
        static_assert(size_t_highest_published_sequence_next_sequence_available_sequence<Sequencer>::value, "Sequencer should have `size_t highest_published_sequence(size_t, size_t)` method");
    }
};
template <class Sequencer, typename Event> using mo_sequencer_t = mo_sequencer_c<Sequencer, Event>;
} // namespace mozi::ring
// clang-format on