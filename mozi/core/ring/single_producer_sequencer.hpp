#pragma once

#include "mozi/core/ring/abstruct_sequencer.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/utils.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace mozi::ring
{
/// 单生产者序列器
// next_value: 下一个要写入的序列
// cache_value: 用户已经处理的最小序列
// cursor: 生产者最后写入的序列
template <class SequenceBarrier, typename Event, class WaitStrategy>
class mo_single_producer_sequencer_c
    : public mo_abstruct_sequencer_c<mo_single_producer_sequencer_c<SequenceBarrier, Event, WaitStrategy>,
                                     SequenceBarrier, Event, WaitStrategy>
{
  public:
    mo_single_producer_sequencer_c(uint16_t buffer_size,
                                   mo_wait_strategy_t<WaitStrategy, SequenceBarrier> wait_strategy)
        : mo_abstruct_sequencer_c<mo_single_producer_sequencer_c<SequenceBarrier, Event, WaitStrategy>, SequenceBarrier,
                                  Event, WaitStrategy>(buffer_size, wait_strategy)
    {
    }
    bool has_available_capacity(uint16_t required_capacity)
    {
        return has_available_capacity(required_capacity, false);
    }
    bool has_available_capacity(uint16_t required_capacity, bool do_store)
    {
        size_t next_value = m_next_value;
        size_t wrap_point = (m_next_value + required_capacity) - this->m_buffer_size;
        size_t cached_gating_sequence = m_cache_value;
        // cached_gating_sequence > next_value 应该只有 size_t 溢出的情况下可能出现
        if (wrap_point > cached_gating_sequence || cached_gating_sequence > next_value)
        {
            if (do_store)
            {
                this->m_cursor.set(next_value);
            }
            size_t min_sequence = mozi::ring::utils::minimum_sequence(this->m_gating_sequences, next_value);
            m_cache_value = min_sequence;
            if (wrap_point > min_sequence)
            {
                return false;
            }
        }
        return true;
    }
    /// 获取生产者下一个要写入的序列
    std::optional<size_t> next()
    {
        return next(1);
    }
    std::optional<size_t> next(uint16_t n) noexcept
    {
#ifdef DEBUG
        // TODO: 这里替换使用 exception
        if (!same_thread())
        {
            throw std::runtime_error("SingleProducerSequencer.next() called from multiple threads");
        }
        if (n < 1 || n > this->m_buffer_size)
        {
            throw std::invalid_argument("n must be greater than 0 and less than buffer_size");
        }
#endif
        size_t next_value = m_next_value;
        size_t next_sequence = next_value + n;
        size_t wrap_point = next_sequence - this->m_buffer_size;
        size_t cached_gating_sequence = m_cache_value;
        if (wrap_point > cached_gating_sequence || cached_gating_sequence > next_value)
        {
            this->m_cursor.set(next_value);
            return std::nullopt;
        }
        m_next_value = next_sequence;
        return next_sequence;
    }
    size_t remaining_capacity() noexcept
    {
        size_t next_value = m_next_value;
        size_t consumed = mozi::ring::utils::minimum_sequence(this->m_gating_sequences, next_value);
        size_t produced = next_value;
        return this->m_buffer_size - (produced - consumed);
    }
    void claim(const size_t sequence) noexcept
    {
        m_next_value = sequence;
    }
    void publish(const size_t sequence) noexcept
    {
        this->m_cursor.set(sequence);
        this->m_wait_strategy.signal_all_when_blocking();
    }
    void publish([[MO_UNUSED]] const size_t lo, const size_t hi) noexcept
    {
        publish(hi);
    }
    bool is_available(const size_t sequence) noexcept
    {
        size_t current_sequence = this->m_cursor.value();
        return (sequence <= current_sequence) && (sequence > current_sequence - this->m_buffer_size);
    }
    size_t highest_published_sequence([[MO_UNUSED]] const size_t lower_bound, const size_t available_sequence)
    {
        return available_sequence;
    }
    // clang-format off
    std::string to_string() noexcept
    {
        std::string gating_sequences{};
        for (auto &sequence : *this->m_gating_sequences.load())
        {
            gating_sequences += sequence->to_string() + ", ";
        }
        return std::string{"mo_single_producer_sequencer_c{"} + 
               "buffer_size=" + std::to_string(this->m_buffer_size) +
               ", wait_strategy=" + this->m_wait_strategy.to_string() +
               ", cursor=" + this->m_cursor.to_string() +
               ", gating_sequences=" + gating_sequences + 
               "}";
    }
    // clang-format on

  private:
    bool same_thread()
    {
#ifdef DEBUG
        return mo__producer_thread_assertion::is_same_thread_producing_to(this);
#endif
        return true;
    }
    size_t m_next_value = mo_sequence_t::INITIAL_VALUE;
    // 用户已经处理的最小序列
    size_t m_cache_value = mo_sequence_t::INITIAL_VALUE;

#ifdef DEBUG
    class mo__producer_thread_assertion
    {
      public:
        static bool is_same_thread_producing_to(SingleProducerSequencer *sequencer)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = producers_.find(sequencer);
            if (it == producers_.end())
            {
                producers_[sequencer] = std::this_thread::get_id();
                return true;
            }
            return it->second == std::this_thread::get_id();
        }

      private:
        static std::unordered_map<SingleProducerSequencer *, std::thread::id> producers_{};
        static std::mutex mutex_;
    }
#endif
};
template <class SequenceBarrier, typename Event, class WaitStrategy>
using mo_single_producer_sequencer_t = mo_single_producer_sequencer_c<SequenceBarrier, Event, WaitStrategy>;
} // namespace mozi::ring