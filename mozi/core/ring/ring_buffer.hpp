#pragma once

#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/alias.hpp"
#include "mozi/core/ring/event_factory.hpp"
#include "mozi/core/ring/event_poller.hpp"
#include "mozi/core/ring/event_sequencer.hpp"
#include "mozi/core/ring/event_sink.hpp"
#include "mozi/core/ring/multi_producer_sequencer.hpp"
#include "mozi/core/ring/poller_factory.hpp"
#include "mozi/core/ring/processing_sequence_barrier.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequence_barrier_factory.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include "mozi/core/ring/single_producer_sequencer.hpp"
#include "spdlog/spdlog.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <sys/types.h>
#include <utility>

namespace mozi::ring
{

template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator> class mo_ring_buffer_s;

template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator>
struct mo_ring_buffer_data_s : public mo_event_factory_t<EventFactory, Event>, public mo_sequencer_t<Sequencer, Event>
{
  public:
    mo_ring_buffer_data_s(std::unique_ptr<Sequencer> sequencer) : m_sequencer{std::move(sequencer)}
    {
        static_assert(Size > 1, "buffer size must > 1");
        // TODO 优化
#define BUFFER_SIZE_MAX (UINT16_MAX + 1)
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#define MAX_ERROR "buffer size must <= " TO_STRING(BUFFER_SIZE_MAX)
        static_assert(Size < UINT16_MAX + 1, MAX_ERROR);
        static_assert((Size & (Size - 1)) == 0, "buffer size must be power of 2");
        // error 这种方法会将所有元素填充为同一个元素的引用 只有一个实例
        // m_entries.fill(EventFactory::create_instance());
        for (auto &entry : m_entries)
        {
            entry = std::move(EventFactory::create_instance());
        }
    }
    Event *operator[](size_t seq)
    {
        return m_entries[seq & m_index_mask_const].get();
    }
    friend class mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>;

  private:
    uint16_t m_index_mask_const = static_cast<uint16_t>(Size - 1);
    std::array<std::unique_ptr<Event>, Size> m_entries;
    uint32_t m_buffer_size = Size;
    std::unique_ptr<Sequencer> m_sequencer;
};

// clang-format off
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator>
class mo_ring_buffer_s : 
    public mo_sequence_barrier_factory_t<mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>, mo_processing_sequence_barrier_t<Sequencer>>,
    public mo_poller_factory_t<mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>, mo_event_poller_t<mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>, Sequencer, Event>>,
    public mo_event_sequencer_t<mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>, Event>,
    public mo_event_sink_t<mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>, Event, Translator>
// clang-format on
// : public mo_cursored_t<mo_ring_buffer_s<Event, Size, Sequencer, SequenceBarrier, EventFactory>>,
{
    using mo_ring_buffer_t = mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>;

  public:
    static constexpr size_t INITIAL_CURSOR_VALUE = mo_sequence_t::INITIAL_VALUE;
    mo_ring_buffer_s(std::unique_ptr<Sequencer> sequencer) : m_data{std::move(sequencer)}
    {
    }

    Event *operator[](size_t seq)
    {
        return this->m_data[seq];
    }

    using single_producer = mo_single_producer_sequencer_t<Event>;
    static std::unique_ptr<mo_ring_buffer_t> create_single_producer()
    {
        std::unique_ptr<single_producer> sequencer = std::make_unique<single_producer>(Size);
        std::unique_ptr<mo_ring_buffer_t> ring_buffer = std::make_unique<mo_ring_buffer_t>(std::move(sequencer));
        return ring_buffer;
    }

    using multi_producer = mo_multi_producer_sequencer_c<Event, Size>;
    static std::unique_ptr<mo_ring_buffer_t> create_multi_producer()
    {
        std::unique_ptr<multi_producer> sequencer = std::make_unique<multi_producer>(Size);
        std::unique_ptr<mo_ring_buffer_t> ring_buffer = std::make_unique<mo_ring_buffer_t>(std::move(sequencer));
        return ring_buffer;
    }

    /**
     * @brief claim unsafe method, just used for sequence overflow test
     */
    size_t claim(const size_t sequence) noexcept
    {
        this->m_data.m_sequencer->claim(sequence);
        return this->m_data.m_sequencer->cursor();
    }

    constexpr uint32_t remaining_capacity() const noexcept
    {
        return this->m_data.m_sequencer->remaining_capacity();
    }

    bool has_available_capacity(uint16_t required_capacity) const noexcept
    {
        return this->m_data.m_sequencer->has_available_capacity(required_capacity);
    }

    constexpr inline uint32_t buffer_size() const noexcept
    {
        return this->m_data.m_buffer_size;
    }

    inline void publish(size_t sequence) noexcept
    {
#ifndef NDEBUG
        spdlog::debug("publish");
#endif
        this->m_data.m_sequencer->publish(sequence);
    }

    inline void publish(size_t lo, size_t hi) noexcept
    {
        this->m_data.m_sequencer->publish(lo, hi);
    }

    inline bool publish_event(Translator &translator) noexcept
    {
        const std::optional<size_t> sequence = this->next();
        if (sequence.has_value()) [[MO_LIKELY]]
        {
            this->translate_and_publish(translator, sequence.value());
            return true;
        }
        return false;
    }

    // TODO: 未测试
    template <typename... Translators> inline bool publish_events(Translators &...translators) noexcept
    {
        auto need_size = sizeof...(Translators);
#ifndef NDEBUG
        if (need_size == 0 || need_size > buffer_size()) [[MO_UNLIKELY]]
        {
            return false;
        };
#endif
        const std::optional<size_t> sequence = this->next(need_size);
        if (!(sequence.has_value())) [[MO_LIKELY]]
        {
            return false;
        }
        translate_and_publish_events(translators..., sequence.value(), need_size);
        return true;
    }
    // TODO: 未测试

    inline std::optional<size_t> next() noexcept
    {
        return this->m_data.m_sequencer->next();
    }

    inline std::optional<size_t> next(uint16_t n) noexcept
    {
        return this->m_data.m_sequencer->next(n);
    }

    template <typename... Sequences> inline void add_gating_sequences(Sequences... sequences) noexcept
    {
        this->m_data.m_sequencer->add_gating_sequences(std::forward<Sequences>(sequences)...);
    }

    inline bool remove_gating_sequence(mo_arc_sequence_t sequence) noexcept
    {
        return this->m_data.m_sequencer->remove_gating_sequences(sequence);
    }

    inline size_t gating_sequences_size() noexcept
    {
        return this->m_data.m_sequencer->gating_sequences_size();
    }

    inline size_t minimum_sequence() const noexcept
    {
        return this->m_data.m_sequencer->minimum_sequence();
    }

    using mo__event_poller_t = mo_event_poller_c<mo_ring_buffer_t, Sequencer, Event>;
    template <typename... Sequences>
    [[MO_NODISCARD]] inline std::unique_ptr<mo__event_poller_t> create_poller(Sequences &&...gating_sequences)
    {
        mo_wait_sequences_t gating_sequence{};
        auto len = sizeof...(Sequences);
        if (len == 0)
        {
            gating_sequence.set_sequences(this->m_data.m_sequencer->cursor_instance());
        }
        else
        {
            gating_sequence.set_sequences(gating_sequences...);
        }
        mo_arc_sequence_t poller_sequence = std::make_shared<mo_sequence_t>();
        this->m_data.m_sequencer->add_gating_sequences(poller_sequence);
        return std::make_unique<mo__event_poller_t>(this,                           // DataProvider *
                                                    this->m_data.m_sequencer.get(), // Sequencer *
                                                    poller_sequence,                // mo_arc_sequence_t
                                                    gating_sequence);               // mo_wait_sequences_t
    }

    // 未稳定

    // TODO: 暂时使用不到 后面可能使用这个创建 每个 actor 可以占用的时间切片
    template <typename... Sequences>
    [[MO_NODISCARD]] inline std::unique_ptr<mo_processing_sequence_barrier_t<Sequencer>> create_barrier(
        [[maybe_unused]] const Sequences &...sequences_to_track) noexcept
    {
        mo_wait_sequences_t gating_sequence{};
        [[maybe_unused]] auto len = sizeof...(Sequences);
        // if (len == 0)
        // {
        //     gating_sequence.set_minimum(this->m_data.m_sequencer->cursor());
        // }
        // else
        // {
        //     gating_sequence.set_sequences(sequences_to_track...);
        // }
        return std::make_unique<mo_processing_sequence_barrier_t<Sequencer>>(this->m_data.m_sequencer.get(),
                                                                             gating_sequence);
    }

  private:
    void translate_and_publish(Translator &translator, size_t sequence) noexcept
    {
        translator((*this)[sequence], sequence);
        this->publish(sequence);
    }

    // TODO: 未测试
    template <typename... Translators>
    void translate_and_publish_events(Translators &...translators, size_t final_sequence, uint16_t batch_size) noexcept
    {
        auto translators_tuple = std::make_tuple(translators...);
        auto const offset = batch_size - 1;
        auto intial_sequence = final_sequence - offset;
        auto const batch_start = 0;
        auto const batch_end = offset;

        for (auto i = batch_start; i <= batch_end; i += 1)
        {
            auto &translator = std::get<i>(translators_tuple);
            translator((*this)[intial_sequence + i], intial_sequence + i);
        }

        this->publish(intial_sequence, final_sequence);
    }
    // TODO: 未测试

    mo_ring_buffer_data_s<Event, Size, Sequencer, EventFactory, Translator> m_data;
};

} // namespace mozi::ring
