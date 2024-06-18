#pragma once
#include "mozi/core/ring/cursored.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequenced.hpp"
#include <cstddef>
#include <type_traits>
namespace mozi::ring
{
template <class DataProvider, class Sequencer, typename Event> class mo_event_poller_c;

template <class SI, typename Event> class mo_sequencer_c : public mo_cursored_t<SI>, public mo_sequenced_t<SI>
{
    template <typename U, typename = void> struct void_claim_sequence : std::false_type
    {
    };
    template <typename U>
    struct void_claim_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().claim(std::declval<size_t>())), void>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_is_available_sequence : std::false_type
    {
    };
    template <typename U>
    struct bool_is_available_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().is_available(std::declval<size_t>())), bool>>>
        : std::true_type
    {
    };

    template <typename U, typename = void, typename... Args>
    struct void_add_gating_sequences_gating_sequences : std::false_type
    {
    };
    template <typename U, typename... Args>
    struct void_add_gating_sequences_gating_sequences<
        U,
        std::enable_if_t<
            std::is_same_v<decltype(std::declval<U>().add_gating_sequences(std::declval<const Args &>()...)), void> &&
            (... && std::is_same_v<Args, mo_sequence_t>)>,
        Args...> : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_remove_gating_sequence_sequence : std::false_type
    {
    };
    template <typename U>
    struct bool_remove_gating_sequence_sequence<
        U, std::enable_if_t<std::is_same_v<
               decltype(std::declval<U>().remove_gating_sequence(std::declval<const mo_sequence_t &>())), bool>>>
        : std::true_type
    {
    };

    template <typename U, typename = void> struct size_t_minimum_sequence : std::false_type
    {
    };
    template <typename U>
    struct size_t_minimum_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().minimum_sequence()), size_t>>> : std::true_type
    {
    };

    template <typename U, typename = void>
    struct size_t_highest_published_sequence_next_sequence_available_sequence : std::false_type
    {
    };
    template <typename U>
    struct size_t_highest_published_sequence_next_sequence_available_sequence<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().highest_published_sequence(
                                               std::declval<size_t>(), std::declval<size_t>())),
                                           size_t>>> : std::true_type
    {
    };

  public:
    size_t INITIAL_CURSOR_VALUE = -1;
    mo_sequencer_c()
    {
        static_assert(void_claim_sequence<SI>::value, "SI should have `void claim(size_t)` method");
        static_assert(bool_is_available_sequence<SI>::value, "SI should have `bool is_available(size_t)` method");
        using sequence_ref = const mo_sequence_t &;
        // TODO 改为宏替换 类似
        // #define SEQUENCE_REF_1 sequence_ref
        // #define SEQUENCE_REF_2 SEQUENCE_REF_1, SEQUENCE_REF_1
        static_assert(
            std::disjunction<
                void_add_gating_sequences_gating_sequences<SI>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref, sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref, sequence_ref, sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref, sequence_ref, sequence_ref,
                                                           sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref, sequence_ref, sequence_ref,
                                                           sequence_ref, sequence_ref>,
                void_add_gating_sequences_gating_sequences<SI, sequence_ref, sequence_ref, sequence_ref, sequence_ref,
                                                           sequence_ref, sequence_ref, sequence_ref>>::value,
            "SI should have `void add_gating_sequences(sequence_ref...)` method");

        static_assert(bool_remove_gating_sequence_sequence<SI>::value,
                      "SI should have `bool remove_gating_sequence(const mo_sequence_t &)` method");

        static_assert(size_t_minimum_sequence<SI>::value, "SI should have `size_t minimum_sequence()` method");
        static_assert(size_t_highest_published_sequence_next_sequence_available_sequence<SI>::value,
                      "SI should have `size_t highest_published_sequence(size_t, size_t)` method");
    }
};
template <class SI, typename Event> using mo_sequencer_t = mo_sequencer_c<SI, Event>;
} // namespace mozi::ring