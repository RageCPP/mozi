#pragma once
#include "mozi/core/ring/abstruct_sequencer.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
namespace mozi::ring
{
// clang-format off
template <typename Event, uint32_t Size>
class mo_multi_producer_sequencer_c : public mo_abstruct_sequencer_c<mo_multi_producer_sequencer_c<Event, Size>, Event>
{
  public:
    void claim(const size_t sequence) noexcept
    {
        this->set_cursor(sequence);
    }
    std::optional<size_t> next() noexcept
    {
        return next(1);
    }
    std::optional<size_t> next(uint16_t n) noexcept
    {
        if (!has_available_capacity(n))
        {
            return std::nullopt;
        }
        auto next_value = this->cursor_instance()->add_and_get(n);
        return next_value;
    }
    bool has_available_capacity(uint16_t required_capacity) noexcept
    {
        return has_available_capacity(this->gating_sequences(), required_capacity, this->cursor());
    }
    bool has_available_capacity(std::vector<mo_arc_sequence_t> *gating_sequences, uint16_t required_capacity, size_t cursor) noexcept
    {
        size_t wrap_point = cursor + required_capacity - this->buffer_size();
        size_t cached_gating_sequence = this->m_gating_sequence_cache.value();

        if (wrap_point > cached_gating_sequence || cached_gating_sequence > cursor) [[MO_UNLIKELY]]
        {
            size_t min_sequence = mozi::ring::utils::minimum_sequence(gating_sequences, cursor);
            this->m_gating_sequence_cache.set(min_sequence);

            if (wrap_point > min_sequence) [[MO_UNLIKELY]]
            {
                return false;
            }
        }
        return true;
    }
    uint32_t remaining_capacity() noexcept
    {
        auto consumed = mozi::ring::utils::minimum_sequence(this->gating_sequences(), this->cursor());
        auto produced = this->cursor();
        return this->buffer_size() - (produced - consumed);
    }
    void publish(const size_t sequence) noexcept
    {
        set_available(sequence);
    }
    void publish(const size_t lo, const size_t hi) noexcept
    {
        for (size_t sequence = lo; sequence <= hi; sequence++)
        {
            set_available(sequence);
        }
    }
    bool is_available(const size_t sequence) noexcept
    {
        auto index = calculate_index(sequence);
        auto flag = calculate_availability_flag(sequence);
        return m_available_buffer[index].load() == flag;
    }
    size_t highest_published_sequence(size_t lower_bound, size_t available_sequence) noexcept
    {
        for (size_t sequence = lower_bound; sequence <= available_sequence; sequence++)
        {
            if (!is_available(sequence))
            {
                return sequence - 1;
            }
        }
        return available_sequence;
    }

  private:
    void set_available(const size_t sequence)
    {
        auto index = calculate_index(sequence);
        auto flag = calculate_availability_flag(sequence);
        set_available_buffer_value(index, flag);
    }
    void set_available_buffer_value(uint16_t index, size_t flag)
    {
        m_available_buffer[index].store(flag);
    }
    size_t calculate_availability_flag(size_t sequence)
    {
        return sequence >> m_index_shift;
    }
    uint16_t calculate_index(size_t sequence)
    {
        return sequence & m_index_mask;
    }

    mo_sequence_t m_gating_sequence_cache {mo_sequencer_t<mo_multi_producer_sequencer_c<Event, Size>, Event>::INITIAL_CURSOR_VALUE};
    std::array<std::atomic<size_t>, Size> m_available_buffer {-1};
    uint16_t m_index_mask = static_cast<uint16_t>(Size - 1);
    uint16_t m_index_shift = std::log2(Size);
};
template <typename Event, uint32_t Size>
using mo_multi_producer_sequencer_t = mo_multi_producer_sequencer_c<Event, Size>;
// clang-format on
} // namespace mozi::ring