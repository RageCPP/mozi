#pragma once

#include "mozi/core/ring/cursored.hpp"
#include "mozi/core/ring/event_factory.hpp"
#include "mozi/core/ring/event_sequencer.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

namespace mozi::ring
{
template <typename Event, uint32_t Size, class Sequencer, class SequencerBarrier> struct mo_ring_buffer_data_s
{
  public:
    mo_ring_buffer_data_s(mo_event_factory_t<Event> event_factory,
                          mo_sequencer_t<Sequencer, SequencerBarrier, Event> sequencer)
        : m_sequencer{sequencer}
    {
        static_assert(Size < 1, "buffer size must > 1");
        // TODO 优化
#define BUFFER_SIZE_MAX (UINT16_MAX + 1)
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

#define MAX_ERROR "buffer size must <= " TO_STRING(BUFFER_SIZE_MAX)
        static_assert(Size > UINT16_MAX + 1, MAX_ERROR);
        static_assert((Size & (Size - 1)) != 0, "buffer size must be power of 2");
        m_entries.fill(event_factory.create_instance());
    }
    const Event &operator[](size_t seq) const
    {
        return m_entries[seq & m_index_mask_const];
    }

  private:
    uint16_t m_index_mask_const = static_cast<uint16_t>(Size - 1);
    std::array<Event, Size> m_entries;
    uint32_t m_buffer_size = Size;
    mo_sequencer_t<Sequencer, SequencerBarrier, Event> m_sequencer;
};
template <typename Event, uint32_t Size, class Sequencer, class SequencerBarrier>
using mo_ring_buffer_data_t = mo_ring_buffer_data_s<Event, Size, Sequencer, SequencerBarrier>;

struct mo_ring_buffer_share_data_s
{
};
using mo_ring_buffer_share_data_t = mo_ring_buffer_share_data_s;

template <typename Event, uint32_t Size, class Sequencer, class SequencerBarrier>
struct mo_ring_buffer_s : public mo_cursored_t<mo_ring_buffer_s<Event, Size, Sequencer, SequencerBarrier>>,
                          public mo_event_sequencer_t<mo_ring_buffer_s<Event, Size, Sequencer, SequencerBarrier>, Event>
{
  public:
    mo_ring_buffer_s(mo_event_factory_t<Event> event_factory,
                     mo_sequencer_t<Sequencer, SequencerBarrier, Event> sequencer)
        : m_data{event_factory, sequencer}
    {
    }

  private:
    mo_ring_buffer_data_t<Event, Size, Sequencer, SequencerBarrier> m_data;
};
} // namespace mozi::ring