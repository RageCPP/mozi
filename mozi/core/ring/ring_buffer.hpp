#pragma once

#include "mozi/compile/attributes_cpp.hpp"
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
        m_entries.fill(EventFactory::create_instance());
    }
    Event &operator[](size_t seq)
    {
        return m_entries[seq & m_index_mask_const];
    }
    friend class mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>;

  private:
    uint16_t m_index_mask_const = static_cast<uint16_t>(Size - 1);
    std::array<Event, Size> m_entries;
    uint32_t m_buffer_size = Size;
    std::unique_ptr<Sequencer> m_sequencer;
};
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator>
using mo_ring_buffer_data_t = mo_ring_buffer_data_s<Event, Size, Sequencer, EventFactory, Translator>;

struct mo_ring_buffer_share_data_s
{
};
using mo_ring_buffer_share_data_t = mo_ring_buffer_share_data_s;

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

    // TODO: 完善
    // static mo_ring_buffer_t *create_multi_producer(
    //     [[maybe_unused]] mo_event_factory_t<EventFactory, Event> event_factory,
    //     [[maybe_unused]] mo_sequencer_t<Sequencer, Event> sequencer)
    // {
    // }
    Event &operator[](size_t seq)
    {
        return this->m_data[seq];
    }
    std::optional<size_t> next() noexcept
    {
        return this->m_data.m_sequencer->next();
    }
    std::optional<size_t> next(uint16_t n) noexcept
    {
        return this->m_data.m_sequencer->next(n);
    }
    uint32_t remaining_capacity() noexcept
    {
        return this->m_data.m_sequencer->remaining_capacity();
    }
    bool has_available_capacity(uint16_t required_capacity) noexcept
    {
        return this->m_data.m_sequencer->has_available_capacity(required_capacity);
    }
    inline uint32_t buffer_size() const noexcept
    {
        return this->m_data.m_buffer_size;
    }
    void publish(size_t sequence) noexcept
    {
        this->m_data.m_sequencer->publish(sequence);
    }
    void publish(size_t lo, size_t hi) noexcept
    {
        this->m_data.m_sequencer->publish(lo, hi);
    }
    void publish_event(Translator &translator) noexcept
    {
        const std::optional<size_t> sequence = this->next();
        if (sequence.has_value()) [[MO_LIKELY]]
        {
            this->translate_and_publish(translator, sequence.value());
        }
    }
    template <typename... Translators> bool publish_events(Translators &...translators) noexcept
    {
        auto need_size = sizeof...(Translators);
#ifdef DEBUG
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
    template <typename... Sequences> inline void add_gating_sequences([[maybe_unused]] Sequences... sequences) noexcept
    {
        // TODO: 功能
    }

    using single_producer = mo_single_producer_sequencer_t<Event>;
    static std::unique_ptr<mo_ring_buffer_t> create_single_producer()
    {
        std::unique_ptr<single_producer> sequencer = std::make_unique<single_producer>(Size);
        std::unique_ptr<mo_ring_buffer_t> ring_buffer = std::make_unique<mo_ring_buffer_t>(std::move(sequencer));
        return ring_buffer;
    }

    using multi_producer = mo_multi_producer_sequencer_t<Event, Size>;
    static std::unique_ptr<mo_ring_buffer_t> create_multi_producer()
    {
        std::unique_ptr<multi_producer> sequencer = std::make_unique<multi_producer>(Size);
        std::unique_ptr<mo_ring_buffer_t> ring_buffer = std::make_unique<mo_ring_buffer_t>(std::move(sequencer));
        return ring_buffer;
    }

    // TODO: 完善
    template <typename... Sequences>
    [[MO_NODISCARD]] inline std::unique_ptr<mo_processing_sequence_barrier_t<Sequencer>> create_barrier(
        [[maybe_unused]] const Sequences &...sequences_to_track) noexcept
    {
        mo_gating_sequences_t gating_sequence{};
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

    using mo__event_poller_t = mo_event_poller_t<mo_ring_buffer_t, Sequencer, Event>;
    template <typename... Sequences>
    [[MO_NODISCARD]] inline std::unique_ptr<mo__event_poller_t> create_poller(Sequences &...gating_sequences)
    {
        static_assert((std::is_same_v<Sequences, mo_arc_sequence_t> && ...),
                      "All Args must be of type mo_arc_sequence_t");
        mo_gating_sequences_t gating_sequence{};
        auto len = sizeof...(Sequences);
        if (len == 0)
        {
            // TODO: 增加 cursor 共享指针计数测试
            gating_sequence.set_sequences(this->m_data.m_sequencer->cursor_instance());
        }
        else
        {
            gating_sequence.set_sequences(gating_sequences...);
        }
        return std::make_unique<mo__event_poller_t>(this, this->m_data.m_sequencer.get(),
                                                    std::make_shared<mo_sequence_t>(), gating_sequence);
    }

  private:
    mo_ring_buffer_data_t<Event, Size, Sequencer, EventFactory, Translator> m_data;

    void translate_and_publish(Translator &translator, size_t sequence) noexcept
    {
        translator((*this)[sequence], sequence);
        this->publish(sequence);
    }

    // clang-format off
    template <typename... Translators>
    void translate_and_publish_events(Translators &...translators, size_t final_sequence, uint16_t batch_size) noexcept
    {
        auto translators_tuple = std::make_tuple(translators...);
        auto intial_sequence = final_sequence - (batch_size - 1);
        const auto batch_start = 0;
        const auto batch_end = batch_size - 1;
        for (auto i = batch_start; i <= batch_end; i += 1)
        {
            auto &translator = std::get<i>(translators_tuple);
            translator((*this)[intial_sequence + i], intial_sequence + i);
        }
        this->publish(intial_sequence, final_sequence);
    }
    // clang-format on
};
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator>
using mo_ring_buffer_t = mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>;
} // namespace mozi::ring
