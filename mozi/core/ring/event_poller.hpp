#pragma once

#include "mozi/core/ring/data_provider.hpp"
#include "mozi/core/ring/gating_sequences.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include <cstddef>
#include <type_traits>

// clang-format off
namespace mozi::ring
{
template <class DataProvider, class Sequencer, typename Event>
class mo_event_poller_c : public mo_sequencer_t<Sequencer, Event>, public mo_data_provider_t<DataProvider, Event>
{
  public:
    explicit mo_event_poller_c(
        DataProvider *data_provider,
        Sequencer *sequencer,
        mo_arc_sequence_t sequence,
        // max sequence to get from the ring buffer for consumer
        mo_gating_sequences_t gating_sequence)
        : m_data_provider(data_provider),
          m_sequencer(sequencer),
          m_sequence(sequence),
          m_gating_sequences(gating_sequence) {}

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

    template <typename I> class mo_handler_c
    {
        // /**
        //  * Called for each event to consume it
        //  *
        //  * @param event the event
        //  * @param sequence the sequence of the event
        //  * @param endOfBatch whether this event is the last in the batch
        //  * @return whether to continue consuming events. If {@code false}, the
        //  * poller will not feed any more events to the handler until {@link
        //  * EventPoller#poll(Handler)} is called again
        //  * @throws Exception any exceptions thrown by the handler will be propagated
        //  * to the caller of {@code poll}
        //  */
        // bool on_event(E event, size_t sequence, bool end_of_batch);
        template<typename U, typename = void>
        struct bool_on_event_event_sequence_end_of_batch : std::false_type
        {
        };
        template<typename U>
        struct bool_on_event_event_sequence_end_of_batch<
            U, std::enable_if_t<
                std::is_same_v<decltype(std::declval<U>().on_event(std::declval<Event &>(), size_t(), bool())), bool>>
        > : std::true_type
        {
        };

        public:
            mo_handler_c()
            {
                static_assert(bool_on_event_event_sequence_end_of_batch<I>::value, "I should have `bool on_event(Event &, size_t, bool)` method");
            };
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
    template <typename Handler> mo_poll_flags poll(Handler event_handler)
    {
        // static_assert(mo_handler_c<Handler>::value, "Handler should have `bool on_event(Event &, size_t, bool)` method");
        size_t current_sequence = m_sequence->value();
        size_t next_sequence = current_sequence + 1;
        size_t available_sequence = m_sequencer->highest_published_sequence(next_sequence, m_gating_sequences.value());
        if (next_sequence <= available_sequence)
        {
            bool process_next_event = true;
            size_t processed_sequence = current_sequence;
            while (next_sequence <= available_sequence && process_next_event)
            {
                Event& event = (*m_data_provider)[next_sequence];
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
    DataProvider *m_data_provider;
    Sequencer *m_sequencer;
    // the sequence used by the event poller [consumed sequence]
    mo_arc_sequence_t m_sequence;
    // the maximum set of sequence events that can be consumed
    mo_gating_sequences_t m_gating_sequences;
};
template <class DataProvider, class Sequencer, typename Event>
using mo_event_poller_t = mo_event_poller_c<DataProvider, Sequencer, Event>;
} // namespace mozi::ring
// clang-format on
