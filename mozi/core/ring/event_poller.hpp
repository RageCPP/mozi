#pragma once

#include "mozi/core/ring/cursored.hpp"
#include "mozi/core/ring/data_provider.hpp"
#include "mozi/core/ring/gating_sequence.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include <cstddef>
#include <memory>

namespace mozi::ring
{
template <class SI, typename Event> class mo_event_poller_c : public mo_cursored_t<SI>, public mo_sequencer_t<SI, Event>
//    public mo_data_provider_t<SI, Event>,
{
  public:
    // clang-format off
    explicit mo_event_poller_c(SI *sequencer,
                               std::unique_ptr<mo_sequence_t> sequence,
                               mo_gating_sequence_t gating_sequence)
        : m_sequencer(sequencer),
          m_sequence(std::move(sequence)),
          m_gating_sequence(gating_sequence) {}
    // clang-format on

    enum class mo_poll_flags
    {
        /**
         * The poller processed one or more events
         */
        MO_POLL_PROCESSING,
        /**
         * The poller is waiting for gated sequences to advance before events become
         * available
         */
        MO_POLL_GATING,
        /**
         * No events need to be processed
         */
        MO_POLL_IDLE
    };

    template <typename E> class mo_handler_c
    {
        /**
         * Called for each event to consume it
         *
         * @param event the event
         * @param sequence the sequence of the event
         * @param endOfBatch whether this event is the last in the batch
         * @return whether to continue consuming events. If {@code false}, the
         * poller will not feed any more events to the handler until {@link
         * EventPoller#poll(Handler)} is called again
         * @throws Exception any exceptions thrown by the handler will be propagated
         * to the caller of {@code poll}
         */
        bool on_event(E event, size_t sequence, bool end_of_batch);
    };

    /**
     * Polls for events using the given handler. <br>
     * <br>
     * This poller will continue to feed events to the given handler until known
     * available events are consumed or {@link Handler#onEvent(Object, long,
     * boolean)} returns false. <br> <br> Note that it is possible for more events
     * to become available while the current events are being processed. A further
     * call to this method will process such events.
     *
     * @param eventHandler the handler used to consume events
     * @return the state of the event poller after the poll is attempted
     * @throws Exception exceptions thrown from the event handler are propagated
     * to the caller
     */
    template <typename E> mo_poll_flags poll(mo_handler_c<E> event_handler)
    {
        size_t current_sequence = m_sequence->value();
        size_t next_sequence = current_sequence + 1;
        size_t available_sequence = m_sequencer->highest_published_sequence(next_sequence, m_gating_sequence.value());

        if (next_sequence <= available_sequence)
        {
            bool process_next_event = true;
            size_t processed_sequence = current_sequence;
            while (next_sequence <= available_sequence && process_next_event)
            {
                E event = (*m_sequencer)[next_sequence];
                process_next_event = event_handler.on_event(event, next_sequence, next_sequence == available_sequence);
                processed_sequence = next_sequence;
                next_sequence++;
            }
            m_sequence->set(processed_sequence);
            return mo_poll_flags::MO_POLL_PROCESSING;
        }
        else if (m_sequencer->cursor() >= next_sequence)
        {
            return mo_poll_flags::MO_POLL_GATING;
        }
        else
        {
            return mo_poll_flags::MO_POLL_IDLE;
        }
    }

    mo_sequence_t *sequence()
    {
        return m_sequence.get();
    }

  private:
    SI *m_sequencer;
    std::unique_ptr<mo_sequence_t> m_sequence;
    mo_gating_sequence_t m_gating_sequence;
};
template <class SI, typename Event> using mo_event_poller_t = mo_event_poller_c<SI, Event>;
} // namespace mozi::ring