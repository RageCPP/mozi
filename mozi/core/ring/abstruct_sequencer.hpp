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

// TODO: 将所有成员变量的共享指针都改为普通指针 将 single_producer_sequencer | multi_producer_sequencer 改为shared_this
// 但是注意这里的m_gating_sequences内部如果存的都是普通指针，那么在以释放的指针时并不知道是否已经释放，但是智能指针其实也不知道
// 需要在想想 需要将被添加的sequence指针资源释放时候自动将其从gating_sequences中移除

namespace mozi::ring
{
template <class SI, typename Event> class mo_abstruct_sequencer_c : public mo_sequencer_t<SI, Event>
{
  public:
    mo_abstruct_sequencer_c(uint32_t buffer_size)
        : m_buffer_size(buffer_size), m_gating_sequences(std::make_shared<std::vector<mo_arc_sequence_t>>())
    {
    }
    template <typename... Sequences> inline void add_gating_sequences(Sequences... sequences) noexcept
    {
        static_assert((std::is_same_v<Sequences, mo_arc_sequence_t &> && ...),
                      "All Args must be of type  std::atomic<mo_arc_sequence_t>");
        mozi::ring::sequence_group::add_sequences(this, std::move(sequences)...);
    }
    inline bool remove_gating_sequence(mo_arc_sequence_t &sequence) noexcept
    {
        return mozi::ring::sequence_group::remove_sequences(*this, sequence);
    }
    inline size_t minimum_sequence() const noexcept
    {
        // TODO: 优化这里是否不使用强一致顺序
        return mozi::ring::utils::minimum_sequence(m_gating_sequences.load().get(), m_cursor->value());
    }
    inline void set_cursor(size_t cursor) noexcept
    {
        m_cursor->set(cursor);
    }
    inline size_t cursor() noexcept
    {
        return m_cursor->value();
    }
    inline uint32_t buffer_size() noexcept
    {
        return m_buffer_size;
    }
    inline std::vector<mo_arc_sequence_t> *gating_sequences() noexcept
    {
        return m_gating_sequences.load().get();
    }
    inline mo_arc_sequence_t cursor_instance() noexcept
    {
        return m_cursor;
    }

    template <class Sequencer, typename... Sequences>
    friend void mozi::ring::sequence_group::add_sequences(Sequencer *sequencer, Sequences &&...sequences);
    template <class Sequencer>
    friend bool mozi::ring::sequence_group::remove_sequences(Sequencer *sequences, mo_arc_sequence_t &sequence);

  private:
    uint32_t m_buffer_size;
    // last sequence that was published.
    mo_arc_sequence_t m_cursor{std::make_shared<mo_sequence_t>(mo_sequencer_t<SI, Event>::INITIAL_CURSOR_VALUE)};
    std::atomic<std::shared_ptr<std::vector<mo_arc_sequence_t>>> m_gating_sequences;
};
} // namespace mozi::ring