#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_barrier.hpp"
#include "mozi/utils/expected.hpp"
#include <cstddef>
#include <type_traits>

namespace mozi::ring
{
template <typename T> T &mo_template_value() noexcept;
struct mo_alerted_true
{
};
template <class I, class SequenceBarrier> class mo_wait_strategy_c
{
  private:
    template <typename U, typename = void>
    struct expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier : std::false_type
    {
    };
    // clang-format off
    template <typename U>
    struct expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier<
        U, std::enable_if_t<
               std::is_same_v<decltype(std::declval<U>().wait_for(
                                  mo_template_value<size_t>(),
                                  std::declval<mo_sequence_t>(),
                                  std::declval<mo_sequence_t>())),
                              mozi::mo_expected_t<size_t, mo_alerted_true>>>> : std::true_type
    {
    };
    // clang-format on

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
        static_assert(expected_size_t_wait_for_sequence_cursir_dependent_sequence_barrier<I>::value,
                      "I should have `mo_expected_t<size_t, mo_alerted_true> wait_for(size_t, mo_sequence_t, "
                      "mo_sequence_t, mo_sequence_barrier_t)` method");
        static_assert(void_signal_all_when_blocking<I>::value,
                      "I should have `void signal_all_when_blocking()` method");
    }
};
template <class I, class SequenceBarrier> using mo_wait_strategy_t = mo_wait_strategy_c<I, SequenceBarrier>;
} // namespace mozi::ring