#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "mozi/utils/expected.hpp"
#include <cstddef>
#include <type_traits>

namespace mozi::ring
{
struct mo_alerted_true;

template <class I> class mo_wait_strategy_c
{
  private:
    template <typename U, typename = void>
    struct expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier : std::false_type
    {
    };
    template <typename U>
    struct expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier<
        U, std::enable_if_t<
               std::is_same_v<decltype(std::declval<U>().wait_for(size_t(), std::declval<const mo_sequence_t &>())),
                              mozi::mo_expected_t<size_t, mo_alerted_true>>>> : std::true_type
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
        // TODO: 根据folly中的traits中的 always_false 改写这里的 SequenceBarrier 是否 实现了 mo_sequence_barrier_t 判断
        static_assert(
            expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier<I>::value,
            "I should have `mo_expected_t<size_t, mo_alerted_true> wait_for(size_t,const mo_sequence_t&)` method");
        static_assert(void_signal_all_when_blocking<I>::value,
                      "I should have `void signal_all_when_blocking()` method");
    }
};
template <class I> using mo_wait_strategy_t = mo_wait_strategy_c<I>;
} // namespace mozi::ring