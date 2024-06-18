#pragma once
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_group.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include "mozi/core/ring/utils.hpp"
#include <atomic>
#include <concurrentqueue.h>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
namespace mozi::ring
{
template <class SI, typename Event> class mo_abstruct_sequencer_c : public mo_sequencer_t<SI, Event>
{
  public:
    mo_abstruct_sequencer_c(uint32_t buffer_size)
        : m_buffer_size(buffer_size),
          m_gating_sequences(std::make_shared<std::vector<std::shared_ptr<mo_sequence_t>>>())
    {
    }
    inline size_t cursor() const noexcept
    {
        return m_cursor.value();
    }
    inline uint32_t buffer_size() const noexcept
    {
        return m_buffer_size;
    }
    template <typename... Args> inline void add_gating_sequences(Args... sequence) noexcept
    {
        static_assert((std::is_same_v<Args, std::shared_ptr<mo_sequence_t>> && ...),
                      "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
        mozi::ring::sequence_group::add_sequences(*this, sequence...);
    }
    inline bool remove_gating_sequence(const mo_sequence_t &sequence) noexcept
    {
        return mozi::ring::sequence_group::remove_sequences(*this, sequence);
    }
    inline size_t minimum_sequence() const noexcept
    {
        // TODO: 优化这里是否不使用强一致顺序
        return mozi::ring::utils::minimum_sequence(*m_gating_sequences.load(), m_cursor.value());
    }

    // clang-format off
    // template <typename... Args>
    // inline decltype(auto) new_poller(mo_data_provider_t<SI, Event> data_provider, Args... gating_sequences) noexcept
    // {
    //     return mo_event_poller_t<SI, mo_processing_sequence_barrier_t<SI, WaitStrategy>, Event>::new_poller(
    //         data_provider,
    //         this,
    //         {},
    //         m_cursor,
    //         gating_sequences...);
    // }

    // inline std::string to_string() noexcept
    // {
    //     std::string gating_sequences{};
    //     for (auto &sequence : *m_gating_sequences.load())
    //     {
    //         gating_sequences += sequence->to_string() + ", ";
    //     }
    //     return std::string{"mo_abstruct_sequencer_c{"} +
    //                             ", wait_strategy=" + m_wait_strategy.to_string() +
    //                             ", cursor=" + std::to_string(m_cursor.value()) + 
    //                             ", gating_sequences=" + gating_sequences + 
    //                             "}";
    // }
    // clang-format on

  protected:
    uint32_t m_buffer_size;
    mo_sequence_t m_cursor{mo_sequencer_t<SI, Event>::INITIAL_CURSOR_VALUE};
    std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> m_gating_sequences;
    // mo_wait_strategy_t<WaitStrategy, mo_processing_sequence_barrier_t<SI, WaitStrategy>> m_wait_strategy;
};
} // namespace mozi::ring