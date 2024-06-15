#pragma once
#include "mozi/core/ring/data_provider.hpp"
#include "mozi/core/ring/sequenced.hpp"
namespace mozi::ring
{
template <class I, typename Event>
class mo_event_sequencer_c : public mo_data_provider_t<I, Event>, public mo_sequenced_t<I>
{
};
template <class I, typename Event> using mo_event_sequencer_t = mo_event_sequencer_c<I, Event>;
} // namespace mozi::ring