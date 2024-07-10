#pragma once
#include "fmt/core.h"
#include "mozi/core/ring/abstruct_sequencer.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include "spdlog/spdlog.h"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
namespace mozi::ring
{
template <typename Event, uint32_t Size>
class mo_multi_producer_sequencer_c : public mo_abstruct_sequencer_c<mo_multi_producer_sequencer_c<Event, Size>, Event>
{
  public:
    mo_multi_producer_sequencer_c(uint32_t buffer_size)
        : mo_abstruct_sequencer_c<mo_multi_producer_sequencer_c, Event>(buffer_size)
    {
    }

    void claim(const size_t sequence) noexcept
    {
        this->set_cursor(sequence);
    }

    std::optional<size_t> next() noexcept
    {
#ifndef NDEBUG
        fmt::println("");
        spdlog::debug("next()");
#endif
        return next(1);
    }

    std::optional<size_t> next(uint16_t n) noexcept
    {
        uint64_t current = 0;
        uint64_t next = 0;
        // TODO: 这里改为 while(true) + break 支持
        do
        {
            current = this->cursor();
            next = current + n;
#ifndef NDEBUG
            spdlog::debug("prev cursor: {}, after cursor: {}", current, next);
#endif
            if (!has_available_capacity(this->gating_sequences(), n, current))
            {
                return std::nullopt;
            }
        } while (!this->cursor_instance()->compare_and_set(current, next));

        return next;
    }

    bool has_available_capacity(uint16_t required_capacity) noexcept
    {
        return has_available_capacity(this->gating_sequences(), required_capacity, this->cursor());
    }

    bool has_available_capacity(std::vector<mo_arc_sequence_t> *gating_sequences, //
                                uint16_t required_capacity,                       //
                                size_t cursor) noexcept
    {
        uint32_t buffer_size = this->buffer_size();
#ifndef NDEBUG
        if (required_capacity > buffer_size)
        {
            spdlog::error("required_capacity: {} buffer_size: {}", required_capacity, buffer_size);
            return false;
        }
#endif
        // TODO: 频繁调用点 是否可优化

        // TODO: 这里改为支持constexpr
        size_t max_offset = buffer_size - required_capacity;
        size_t will_write = cursor + required_capacity;
        size_t can_write = this->m_gating_sequence_cache.value() + 1;
        size_t real_offset = (will_write >= can_write)                      //
                                 ? (will_write - can_write)                 //
                                 : (will_write + SIZE_MAX - can_write + 1); //
#ifndef NDEBUG
        if (!(real_offset > max_offset))
        {
            spdlog::info("can_write: {} will_write: {} max_offset: {} real_offset: {}",
                         can_write,    //
                         will_write,   //
                         max_offset,   //
                         real_offset); //
        }
#endif

        if (real_offset > max_offset) [[MO_UNLIKELY]]
        {
            size_t min_sequence = mozi::ring::utils::minimum_sequence(gating_sequences, cursor);
            this->m_gating_sequence_cache.set(min_sequence);
            can_write = this->m_gating_sequence_cache.value() + 1;
            real_offset = (will_write >= can_write)                      //
                              ? (will_write - can_write)                 //
                              : (will_write + SIZE_MAX - can_write + 1); //

#ifndef NDEBUG
            spdlog::debug("min_sequence: {}", min_sequence);
            spdlog::info("can_write: {} max_offset: {} real_offset: {}", can_write, max_offset, real_offset);
#endif

            if (real_offset > max_offset)
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

#ifndef NDEBUG
        spdlog::debug("available: index: {}, flag: {}", index, flag);
#endif
    }
    void set_available_buffer_value(uint16_t index, size_t flag)
    {
        // TODO: 内存顺序优化
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

    mo_sequence_t m_gating_sequence_cache{mo_sequencer_t<mo_multi_producer_sequencer_c, Event>::INITIAL_CURSOR_VALUE};
    std::array<std::atomic<size_t>, Size> m_available_buffer{-1};
    uint16_t m_index_mask = static_cast<uint16_t>(Size - 1);
    uint16_t m_index_shift = std::log2(Size);
};
} // namespace mozi::ring