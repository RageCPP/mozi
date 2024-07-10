#pragma once

#include "mozi/core/ring/data_provider.hpp"
#include "mozi/core/ring/gating_sequences.hpp"
#include "mozi/core/ring/sequence.hpp"
#include "mozi/core/ring/sequencer.hpp"
#include <cstddef>
#include <type_traits>

namespace mozi::ring
{
template <class DataProvider, class Sequencer, typename Event>
class mo_event_poller_c : public mo_sequencer_t<Sequencer, Event>, public mo_data_provider_t<DataProvider, Event>
{
  public:
    explicit mo_event_poller_c(DataProvider *data_provider,         //
                               Sequencer *sequencer,                //
                               mo_arc_sequence_t sequence,          //
                               mo_wait_sequences_t gating_sequence) //
        : m_data_provider(data_provider),                           //
          m_sequencer(sequencer),                                   //
          m_sequence(sequence),                                     //
          m_wait_sequences(gating_sequence)                         //
    {
    }

    ~mo_event_poller_c()
    {
        m_sequencer->remove_gating_sequences(m_sequence);
    }

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
        template <typename U, typename = void> struct bool_on_event_event_sequence_end_of_batch : std::false_type
        {
        };
        template <typename U>
        struct bool_on_event_event_sequence_end_of_batch<
            U,
            std::enable_if_t<std::is_same_v<decltype(std::declval<U>().on_event(new Event(), size_t(), bool())), bool>>>
            : std::true_type
        {
        };

      public:
        mo_handler_c()
        {
            static_assert(bool_on_event_event_sequence_end_of_batch<I>::value,
                          "I should have `bool on_event(Event *, size_t, bool)` method");
        };
    };

    template <typename Handler> mo_poll_flags poll([[maybe_unused]] Handler event_handler)
    {

        size_t current_sequence = m_sequence->value();
        size_t next_sequence = current_sequence + 1;
        size_t available_sequence = m_sequencer->highest_published_sequence(next_sequence, m_wait_sequences.value());
        if (next_sequence <= available_sequence)
        {
            [[maybe_unused]] bool process_next_event = false;
            size_t processed_sequence = current_sequence;
            DataProvider &data = *m_data_provider;
            while (true)
            {
                Event *event = data[next_sequence];
                // if (event != nullptr)
                // { // 确保event不是空指针
                //     std::cout << typeid(*event).name() << std::endl;
                //     std::cout << "event is not nullptr" << std::endl;
                // }
                // else
                // {
                //     std::cout << "event is nullptr" << std::endl;
                // }
                process_next_event = event_handler.on_event(event, next_sequence, next_sequence == available_sequence);
                processed_sequence = next_sequence;
                next_sequence += 1;

                if (!(next_sequence <= available_sequence && process_next_event))
                {
                    break;
                }
            }
            m_sequence->set(processed_sequence);
            return mo_poll_flags::MO_POLL_PROCESSING;
        }
        else if (m_sequencer->cursor() >= next_sequence)
        {
            // 只有在非使用 sequencer cursor 作为 wait sequence 的情况下才可能会进入这个分支
            return mo_poll_flags::MO_POLL_GATING;
        }
        else
        {
            return mo_poll_flags::MO_POLL_IDLE;
        }
    }

    mo_arc_sequence_t sequence()
    {
        return m_sequence;
    }

  private:
    DataProvider *m_data_provider;
    Sequencer *m_sequencer;
    mo_arc_sequence_t m_sequence;
    mo_wait_sequences_t m_wait_sequences;
};

} // namespace mozi::ring
