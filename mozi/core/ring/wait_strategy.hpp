#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_barrier.hpp"
#include <cstddef>
#include <type_traits>

namespace mozi::ring
{
template <class T, class SequenceBarrier> class mo_wait_strategy_c
{
    template <typename U, typename = void>
    struct size_t_wait_for_sequence_cursir_dependent_sequence_barrier : std::false_type
    {
    };
    template <typename U>
    struct size_t_wait_for_sequence_cursir_dependent_sequence_barrier<
        U, std::enable_if_t<
               std::is_same_v<decltype(std::declval<U>().wait_for(
                                  std::declval<size_t>(), std::declval<mo_sequence_t>(), std::declval<mo_sequence_t>(),
                                  std::declval<mo_sequence_barrier_t<SequenceBarrier>>())),
                              size_t>>> : std::true_type
    {
    };

    template <typename U, typename = void> struct void_signal_all_when_blocking : std::false_type
    {
    };
    template <typename U>
    struct void_signal_all_when_blocking<
        U, std::enable_if_t<std::is_same_v<decltype(std::declval<U>().signal_all_when_blocking()), void>>>
        : std::true_type
    {
    };

  public:
    mo_wait_strategy_c()
    {
        static_assert(size_t_wait_for_sequence_cursir_dependent_sequence_barrier<T>::value,
                      "T should have `size_t wait_for(size_t, mo_sequence_t, mo_sequence_t, "
                      "mo_sequence_barrier_t<SequenceBarrier>)` method");
        static_assert(void_signal_all_when_blocking<T>::value,
                      "T should have `void signal_all_when_blocking()` method");
    }
};
template <class T, class SequenceBarrier> using mo_wait_strategy_t = mo_wait_strategy_c<T, SequenceBarrier>;
} // namespace mozi::ring