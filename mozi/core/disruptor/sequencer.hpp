#pragma once
#include "mozi/core/disruptor/cursored.hpp"
#include "mozi/core/disruptor/data_provider.hpp"
#include "mozi/core/disruptor/sequence.hpp"
#include "mozi/core/disruptor/sequence_barrier.hpp"
#include "mozi/core/disruptor/sequenced.hpp"
#include <cstddef>
#include <memory>
#include <type_traits>
namespace mozi::disruptor
{
template <class Sequencer, class SequenceBarrier, template <auto> class DataProvider> class mo_event_poller_c;

template <class T, class SequenceBarrier, template <auto> class DataProvider>
class mo_sequencer_c : public mo_cursored_t<T>, public mo_sequenced_t<T>
{
    size_t INITIAL_CURSOR_VALUE = -1;
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
            std::is_same_v<decltype(std::declval<U>().add_gating_sequences(std::declval<Args>()...)), void> &&
            (... && std::is_same_v<Args, std::shared_ptr<mo_sequence_t>>)>,
        Args...> : std::true_type
    {
    };

    template <typename U, typename = void> struct bool_remove_gating_sequence_sequence : std::false_type
    {
    };
    template <typename U>
    struct bool_remove_gating_sequence_sequence<
        U,
        std::enable_if_t<std::is_same_v<
            decltype(std::declval<U>().remove_gating_sequence(std::declval<std::shared_ptr<mo_sequence_t>>())), bool>>>
        : std::true_type
    {
    };

    template <typename U, typename = void, typename... Args>
    struct mo_sequence_barrier_t_new_barrier_sequences_to_track : std::false_type
    {
    };
    template <typename U, typename... Args>
    struct mo_sequence_barrier_t_new_barrier_sequences_to_track<
        U,
        std::enable_if_t<std::is_same_v<decltype(std::declval<U>().new_barrier(std::declval<Args>()...)),
                                        mo_sequence_barrier_t<SequenceBarrier>> &&
                         (... && std::is_same_v<Args, std::shared_ptr<mo_sequence_t>>)>,
        Args...> : std::true_type
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

    template <typename U, typename = void, typename... Args>
    struct mo_event_poller_t_new_poller_provider_gatingsequences : std::false_type
    {
    };
    template <typename U, typename... Args>
    struct mo_event_poller_t_new_poller_provider_gatingsequences<
        U,
        std::enable_if_t<std::is_same_v<decltype(std::declval<U>().new_poller(
                                            std::declval<mo_data_provider_t<DataProvider>>(), std::declval<Args>()...)),
                                        mo_event_poller_c<T, SequenceBarrier, DataProvider> *> &&
                         (... && std::is_same_v<Args, std::shared_ptr<mo_sequence_t>>)>,
        Args...> : std::true_type
    {
    };

  public:
    mo_sequencer_c()
    {
        static_assert(void_claim_sequence<T>::value, "T should have `void claim(size_t)` method");
        static_assert(bool_is_available_sequence<T>::value, "T should have `bool is_available(size_t)` method");
        static_assert(
            std::disjunction<
                void_add_gating_sequences_gating_sequences<T, std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<T, std::shared_ptr<mo_sequence_t>,
                                                           std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                void_add_gating_sequences_gating_sequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>>::value,
            "T should have `void add_gating_sequences(std::shared_ptr<mo_sequence_t>...)` method");
        static_assert(bool_remove_gating_sequence_sequence<T>::value,
                      "T should have `bool remove_gating_sequence(std::shared_ptr<mo_sequence_t>)` method");
        static_assert(
            std::disjunction<
                mo_sequence_barrier_t_new_barrier_sequences_to_track<T, std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<T, std::shared_ptr<mo_sequence_t>,
                                                                     std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_sequence_barrier_t_new_barrier_sequences_to_track<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>>::value,
            "T should have `mo_sequence_barrier_t new_barrier(std::shared_ptr<mo_sequence_t>...)` method");
        static_assert(size_t_minimum_sequence<T>::value, "T should have `size_t minimum_sequence()` method");
        static_assert(size_t_highest_published_sequence_next_sequence_available_sequence<T>::value,
                      "T should have `size_t highest_published_sequence(size_t, size_t)` method");
        static_assert(
            std::disjunction<
                mo_event_poller_t_new_poller_provider_gatingsequences<T, std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<T, std::shared_ptr<mo_sequence_t>,
                                                                      std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>>,
                mo_event_poller_t_new_poller_provider_gatingsequences<
                    T, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>, std::shared_ptr<mo_sequence_t>,
                    std::shared_ptr<mo_sequence_t>>>::value,
            "T should have `mo_event_poller_t* new_poller(mo_data_provider_t<DataProvider>, "
            "std::shared_ptr<mo_sequence_t>...)` method");
    }
};
template <class T, class SequenceBarrier, template <auto> class DataProvider>
using mo_sequencer_t = mo_sequencer_c<T, SequenceBarrier, DataProvider>;
} // namespace mozi::disruptor