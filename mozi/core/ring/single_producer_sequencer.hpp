#pragma once
#include "mozi/core/ring/abstruct_sequencer.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/utils.hpp"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <unordered_map>

namespace mozi::ring
{
/// 单生产者序列器
// m_next_value: 下一个要写入的序列
// m_cached_value: 用户已经处理的最小序列
// cursor: 生产者最后写入的序列
template <typename Event>
class mo_single_producer_sequencer_c : public mo_abstruct_sequencer_c<mo_single_producer_sequencer_c<Event>, Event>
{
  public:
    mo_single_producer_sequencer_c(uint32_t buffer_size)
        : mo_abstruct_sequencer_c<mo_single_producer_sequencer_c<Event>, Event>(buffer_size)
    {
    }
    bool has_available_capacity(uint16_t required_capacity) noexcept
    {
        return has_available_capacity(required_capacity, false);
    }
    bool has_available_capacity(uint16_t required_capacity, bool do_store) noexcept
    {
        size_t next_value = m_next_value;
        // wrap_point: new ring buffer safe start point
        size_t wrap_point = next_value + required_capacity - this->buffer_size();
        size_t cached_gating_sequence = m_cached_value;
        // wrap_point > cached_gating_sequence: new minimum safe start point is greater than the not consumed sequence
        // cached_gating_sequence > next_value: size_t sequence overflow, loopback
        if (wrap_point > cached_gating_sequence || cached_gating_sequence > next_value) [[MO_UNLIKELY]]
        {
            this->set_cursor(static_cast<uint16_t>(do_store) * next_value +
                             static_cast<uint16_t>(!do_store) * this->cursor());

            size_t min_sequence = mozi::ring::utils::minimum_sequence(this->gating_sequences(), next_value);
            m_cached_value = min_sequence;

            if (wrap_point > min_sequence) [[MO_UNLIKELY]]
            {
                return false;
            }
        }
        return true;
    }
    /// 获取生产者下一个要写入的序列
    std::optional<size_t> next() noexcept
    {
        return next(1);
    }
    std::optional<size_t> next(uint16_t n) noexcept
    {
#ifndef NDEBUG
        // TODO: 这里替换使用 exception
        // if (!same_thread())
        // {
        //     throw std::runtime_error("SingleProducerSequencer.next() called from multiple threads");
        // }
        // if (n < 1 || n > this->m_buffer_size)
        // {
        //     throw std::invalid_argument("n must be greater than 0 and less than buffer_size");
        // }
#endif
        if (!has_available_capacity(n, true)) [[MO_UNLIKELY]]
        {
            return std::nullopt;
        }
        auto next_sequence = this->m_next_value += n;
        return next_sequence;
    }
    uint32_t remaining_capacity() noexcept
    {
        size_t next_value = m_next_value;
        size_t consumed = mozi::ring::utils::minimum_sequence(*this->gating_sequences(), next_value);
        size_t produced = next_value;
        return this->m_buffer_size - (produced - consumed);
    }
    void claim(const size_t sequence) noexcept
    {
        m_next_value = sequence;
    }
    void publish(const size_t sequence) noexcept
    {
        this->set_cursor(sequence);
        // not use signal_all_when_blocking, beacuse use yield strategy, not block
        // this->m_wait_strategy.signal_all_when_blocking();
    }
    void publish([[MO_UNUSED]] const size_t lo, const size_t hi) noexcept
    {
        publish(hi);
    }
    bool is_available(const size_t sequence) noexcept
    {
        size_t last_publish_sequence = this->cursor();
        return (sequence <= last_publish_sequence) && (sequence > (last_publish_sequence - this->m_buffer_size));
    }
    size_t highest_published_sequence([[MO_UNUSED]] const size_t lower_bound, const size_t available_sequence)
    {
        return available_sequence;
    }

  private:
    bool same_thread()
    {
#ifndef NDEBUG
        return mo__producer_thread_assertion::is_same_thread_producing_to(this);
#endif
        return true;
    }
    size_t m_next_value = mo_sequence_t::INITIAL_VALUE;
    // 用户已经处理的最小序列
    size_t m_cached_value = mo_sequence_t::INITIAL_VALUE;

#ifndef NDEBUG
    class mo__producer_thread_assertion
    {
      public:
        static bool is_same_thread_producing_to(mo_single_producer_sequencer_c *sequencer)
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
        static std::unordered_map<mo_single_producer_sequencer_c *, std::thread::id> producers_{};
        static std::mutex mutex_;
    };
#endif
};
template <typename Event> using mo_single_producer_sequencer_t = mo_single_producer_sequencer_c<Event>;
} // namespace mozi::ring