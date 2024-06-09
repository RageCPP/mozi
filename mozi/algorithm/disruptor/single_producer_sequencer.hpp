#pragma once

#include "mozi/algorithm/disruptor/sequenced.hpp"
#include <cstddef>
#include <cstdint>

namespace mozi::disruptor
{
class mo_single_producer_sequencer_c : public mo_sequenced_t<mo_single_producer_sequencer_c>
{
  public:
    /**
     * Attempt to claim the next n events in sequence for publishing.  Will return the
     * highest numbered slot if there is at least <code>requiredCapacity</code> slots
     * available.  Have a look at {@link Sequencer#next()} for a description on how to
     * use this method.
     *
     * @param n the number of sequences to claim
     * @return the claimed sequence value
     * @throws InsufficientCapacityException thrown if there is no space available in the ring buffer.
     */
    size_t try_next([[maybe_unused]] uint16_t n)
    {
        return 0;
    }
    /**
     * Publishes a sequence. Call when the event has been filled.
     *
     * @param sequence the sequence to be published.
     */
    void publish([[maybe_unused]] size_t sequence)
    {
    }
    /**
     * Batch publish sequences.  Called when all of the events have been filled.
     *
     * @param lo first sequence number to publish
     * @param hi last sequence number to publish
     */
    void publish([[maybe_unused]] size_t lo, [[maybe_unused]] size_t hi)
    {
    }
};
using mo_single_producer_sequencer_t = mo_single_producer_sequencer_c;
} // namespace mozi::disruptor