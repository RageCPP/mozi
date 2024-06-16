#pragma once
#include "mozi/core/ring/data_provider.hpp"
#include "mozi/core/ring/event_poller.hpp"
#include "mozi/core/ring/processing_sequence_barrier.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_group.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include "mozi/core/ring/utils.hpp"
#include "mozi/core/ring/wait_strategy.hpp"
#include <atomic>
#include <concurrentqueue.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace mozi::ring
{
template <class T, typename Event, class WaitStrategy>
class mo_abstruct_sequencer_c : public mo_sequencer_t<T, mo_processing_sequence_barrier_t<T, WaitStrategy>, Event>,
                                public std::enable_shared_from_this<mo_abstruct_sequencer_c<T, Event, WaitStrategy>>
{
  public:
    mo_abstruct_sequencer_c(
        uint16_t buffer_size,
        mo_wait_strategy_t<WaitStrategy, mo_processing_sequence_barrier_t<T, WaitStrategy>> wait_strategy)
        : m_buffer_size(buffer_size), m_wait_strategy(wait_strategy),
          m_gating_sequences(std::make_shared<std::vector<std::shared_ptr<mo_sequence_t>>>())
    {
    }
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
        mozi::ring::sequence_group::add_sequences(*this, sequence...);
    }
    inline bool remove_gating_sequence(std::shared_ptr<mo_sequence_t> sequence) noexcept
    {
        return mozi::ring::sequence_group::remove_sequences(*this, sequence);
    }
    inline size_t minimum_sequence() noexcept
    {
        // TODO: 优化这里是否不使用强一致顺序
        return mozi::ring::utils::minimum_sequence(*m_gating_sequences.load(), m_cursor.value());
    }
    template <typename... Args> inline auto new_barrier(Args... sequences_to_track) noexcept
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
        return mo_processing_sequence_barrier_t<T, WaitStrategy>(this, gating_sequence, &m_wait_strategy, &m_cursor);
    }

    // clang-format off
    template <typename... Args>
    inline decltype(auto) new_poller(mo_data_provider_t<T, Event> data_provider, Args... gating_sequences) noexcept
    {
        return mo_event_poller_t<T, mo_processing_sequence_barrier_t<T, WaitStrategy>, Event>::new_poller(
            data_provider,
            this,
            {},
            m_cursor,
            gating_sequences...);
    }

    inline std::string to_string() noexcept
    {
        std::string gating_sequences{};
        for (auto &sequence : *m_gating_sequences.load())
        {
            gating_sequences += sequence->to_string() + ", ";
        }
        return std::string{"mo_abstruct_sequencer_c{"} +
                                ", wait_strategy=" + m_wait_strategy.to_string() +
                                ", cursor=" + std::to_string(m_cursor.value()) + 
                                ", gating_sequences=" + gating_sequences + 
                                "}";
    }
    // clang-format on

  protected:
    uint16_t m_buffer_size;
    mo_wait_strategy_t<WaitStrategy, mo_processing_sequence_barrier_t<T, WaitStrategy>> m_wait_strategy;
    mo_sequence_t m_cursor{
        mo_sequencer_t<T, mo_processing_sequence_barrier_t<T, WaitStrategy>, Event>::INITIAL_CURSOR_VALUE};
    std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> m_gating_sequences;
};
} // namespace mozi::ring