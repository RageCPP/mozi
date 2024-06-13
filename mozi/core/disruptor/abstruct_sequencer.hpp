#pragma once
#include "mozi/core/disruptor/processing_sequence_barrier.hpp"
#include "mozi/core/disruptor/sequence.hpp"
#include "mozi/core/disruptor/sequence_barrier.hpp"
#include "mozi/core/disruptor/sequence_group.hpp"
#include "mozi/core/disruptor/sequencer.hpp"
#include "mozi/core/disruptor/utils.hpp"
#include "mozi/core/disruptor/wait_strategy.hpp"
#include <atomic>
#include <concurrentqueue.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace mozi::disruptor
{
template <class T, class SequenceBarrier, template <auto> class DataProvider, class WaitStrategy>
class mo_abstruct_sequencer_c
    : public mo_sequencer_t<T, SequenceBarrier, DataProvider>,
      public std::enable_shared_from_this<mo_abstruct_sequencer_c<T, SequenceBarrier, DataProvider, WaitStrategy>>
{
  public:
    inline size_t cursor() noexcept
    {
        return m_cursor.value();
    }
    inline uint16_t buffer_size() noexcept
    {
        return m_buffer_size;
    }
    template <typename... Args> inline void add_gating_sequences(Args... sequence) noexcept
    {
        static_assert((std::is_same_v<Args, std::shared_ptr<mo_sequence_t>> && ...),
                      "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
        mozi::disruptor::sequence_group::add_sequences(*this, sequence...);
    }
    inline bool remove_gating_sequence(std::shared_ptr<mo_sequence_t> sequence) noexcept
    {
        return mozi::disruptor::sequence_group::remove_sequences(*this, sequence);
    }
    inline size_t minimum_sequence() noexcept
    {
        // TODO: 优化这里是否不使用强一致顺序
        return mozi::disruptor::utils::minimum_sequence(*m_gating_sequences.load(), m_cursor.value());
    }
    template <typename... Args>
    inline mo_sequence_barrier_t<mo_processing_sequence_barrier_t<T, DataProvider, WaitStrategy>> new_barrier(
        Args... sequences_to_track) noexcept
    {
        static_assert((std::is_same_v<Args, mo_sequence_t> && ...), "All Args must be of type mo_sequence_t");
        mo_gating_sequence_t gating_sequence{};
        auto len = sizeof...(Args);
        if (len == 0)
        {
            gating_sequence.set_minimum(m_cursor.value());
        }
        else
        {
            gating_sequence.set_sequences(sequences_to_track...);
        }
        return mo_processing_sequence_barrier_t<T, DataProvider, WaitStrategy>(this, gating_sequence, &m_wait_strategy,
                                                                               &m_cursor);
    }

  protected:
    uint16_t m_buffer_size;
    mo_wait_strategy_t<WaitStrategy, SequenceBarrier> m_wait_strategy;
    mo_sequence_t m_cursor{mo_sequencer_t<T, SequenceBarrier, DataProvider>::INITIAL_CURSOR_VALUE};
    std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> m_gating_sequences;
};
} // namespace mozi::disruptor