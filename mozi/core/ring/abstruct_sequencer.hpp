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
    template <typename... Sequences> inline void add_gating_sequences(Sequences... sequences) noexcept
    {
        static_assert((std::is_same_v<Sequences, std::shared_ptr<mo_sequence_t>> && ...),
                      "All Args must be of type  std::atomic<std::shared_ptr<mo_sequence_t>>");
        mozi::ring::sequence_group::add_sequences(this, std::move(sequences)...);
    }
    inline bool remove_gating_sequence(const mo_sequence_t &sequence) noexcept
    {
        return mozi::ring::sequence_group::remove_sequences(*this, sequence);
    }
    inline size_t minimum_sequence() const noexcept
    {
        // TODO: 优化这里是否不使用强一致顺序
        return mozi::ring::utils::minimum_sequence(m_gating_sequences.load().get(), m_cursor.value());
    }
    inline void set_cursor(size_t cursor) noexcept
    {
        m_cursor.set(cursor);
    }
    inline size_t cursor() noexcept
    {
        return m_cursor.value();
    }
    inline uint32_t buffer_size() noexcept
    {
        return m_buffer_size;
    }
    inline std::vector<std::shared_ptr<mo_sequence_t>> *gating_sequences() noexcept
    {
        return m_gating_sequences.load().get();
    }
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
    template <class Sequencer, typename... Sequences>
    friend void mozi::ring::sequence_group::add_sequences(Sequencer *sequencer, Sequences &&...sequences);
    template <class Sequencer>
    friend bool mozi::ring::sequence_group::remove_sequences(Sequencer *sequences,
                                                             std::shared_ptr<mo_sequence_t> &sequence);

  private:
    uint32_t m_buffer_size;
    // last sequence that was published.
    mo_sequence_t m_cursor{mo_sequencer_t<SI, Event>::INITIAL_CURSOR_VALUE};
    std::atomic<std::shared_ptr<std::vector<std::shared_ptr<mo_sequence_t>>>> m_gating_sequences;
    // mo_wait_strategy_t<WaitStrategy, mo_processing_sequence_barrier_t<SI, WaitStrategy>> m_wait_strategy;
};
} // namespace mozi::ring