#pragma once

#include "mozi/algorithm/disruptor/data_provider.hpp"
#include "mozi/algorithm/disruptor/sequence.hpp"
namespace mozi::disruptor
{
template <template <auto> class A> class mo_event_poller_c
{
  private:
    mo_data_provider_t<A> m_data_provider;
    mo_sequence_t m_sequence;
};
} // namespace mozi::disruptor