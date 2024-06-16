#pragma once

#include "mozi/core/ring/gating_sequence.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_barrier.hpp"
#include "mozi/core/ring/wait_strategy.hpp"
#include <atomic>
namespace mozi::ring
{
template <class SI, class WaitStrategy>
class mo_processing_sequence_barrier_c
    : public mo_sequence_barrier_t<mo_processing_sequence_barrier_c<SI, WaitStrategy>>
{
    using mo__wait_strategy = mo_wait_strategy_t<WaitStrategy, mo_processing_sequence_barrier_c>;

  public:
    // clang-format off
    mo_processing_sequence_barrier_c(
      SI *sequencer,
      mo_gating_sequence_t dependent_sequence,
      mo__wait_strategy *wait_strategy,
      mo_sequence_t *cursor_sequence) noexcept
        : m_sequencer(sequencer),
          m_dependent_sequence(dependent_sequence),
          m_wait_strategy(wait_strategy),
          m_cursor_sequence(cursor_sequence) {}
    // clang-format on

  private:
    SI *m_sequencer;
    mo_gating_sequence_t m_dependent_sequence;
    std::atomic<bool> alerted = false;
    mo__wait_strategy *m_wait_strategy;
    mo_sequence_t *m_cursor_sequence;
};
template <class SI, class WaitStrategy>
using mo_processing_sequence_barrier_t = mo_processing_sequence_barrier_c<SI, WaitStrategy>;
} // namespace mozi::ring
