#pragma once

#include "mozi/core/ring/gating_sequences.hpp"
#include "mozi/core/ring/sequence_barrier.hpp"
#include <atomic>
namespace mozi::ring
{
struct mo_alerted_true
{
};
// TODO: alerted 读取与写入是否可以使用内存顺序优化 避免不必要的内存同步
// TODO: 未仔细思考
template <class SI>
class mo_processing_sequence_barrier_c : public mo_sequence_barrier_t<mo_processing_sequence_barrier_c<SI>>
{
  public:
    // clang-format off
    mo_processing_sequence_barrier_c(
      SI *sequencer,
      mo_wait_sequences_t dependent_sequence) noexcept
        : m_sequencer(sequencer),
          m_dependent_sequence(dependent_sequence) {}
    // clang-format on

    size_t wait_for(size_t sequence) noexcept
    {
        return m_sequencer->highest_published_sequence(sequence, m_dependent_sequence.value());
    }

    void clear_alert() noexcept
    {
        alerted.store(false);
    }

    void alert() noexcept
    {
        alerted.store(true);
    }

    bool is_alerted() const noexcept
    {
        return alerted.load();
    }

    size_t cursor() const noexcept
    {
        return m_dependent_sequence.value();
    }

  private:
    SI *m_sequencer;
    mo_wait_sequences_t m_dependent_sequence;
    std::atomic<bool> alerted = false;
};
template <class SI> using mo_processing_sequence_barrier_t = mo_processing_sequence_barrier_c<SI>;
} // namespace mozi::ring
