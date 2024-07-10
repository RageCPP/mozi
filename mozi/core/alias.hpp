#pragma once
#include "mozi/core/mail.hpp"
#include "mozi/nid/worker.hpp"
#include "mozi/variables/const_dec.hpp"
#include <cstdint>
#include <libcuckoo/cuckoohash_map.hh>
#include <r3.h>
namespace mozi
{
namespace http
{
using mo_responder_t = class mo_responder_c;
}
using mo_route_t = R3Node;
/** aliases for types in libcuckoo */
using mo_responder_map_t = libcuckoo::cuckoohash_map<int64_t, std::shared_ptr<http::mo_responder_t>>;
struct mo_loop_data_s
{
    uint8_t loop_id;
    mo_route_t *rageroute;
    mo_responder_map_t *responders;
    nid::mo_nid_worker_t *nid_worker;
};
using mo_loop_data_t = mo_loop_data_s;

namespace actor
{
template <uint32_t Size> class mo_uv_actor_c;
template <uint32_t Size> class mo_actor_c;
}; // namespace actor

namespace mail
{
struct mo_mail_s;
class mo_mail_factory_c;
class mo_mail_translator_c;
class mo_mail_read_c;
}; // namespace mail

namespace ring
{
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator> //
struct mo_ring_buffer_data_s;                                                                   //
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator> //
class mo_ring_buffer_s;                                                                         //
template <typename Event, uint32_t Size>                                                        //
class mo_multi_producer_sequencer_c;                                                            //
template <class DataProvider, class Sequencer, typename Event>                                  //
class mo_event_poller_c;                                                                        //
}; // namespace ring

using mo_uv_actor_t = actor::mo_uv_actor_c<BITS_13>;

using mo_actor_t = actor::mo_actor_c<BITS_8>;

using mo_mail_t = mail::mo_mail_s;

using mo_mail_factory_t = mail::mo_mail_factory_c;

using mo_mail_translator_t = mail::mo_mail_translator_c;

using mo_mail_read_t = mail::mo_mail_read_c;

template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator>
using mo_ring_buffer_t = ring::mo_ring_buffer_s<Event, Size, Sequencer, EventFactory, Translator>;

template <typename Event, uint32_t Size>
using mo_multi_producer_sequencer_t = ring::mo_multi_producer_sequencer_c<Event, Size>;

template <class DataProvider, class Sequencer, typename Event>
using mo_event_poller_t = ring::mo_event_poller_c<DataProvider, Sequencer, Event>;

// clang-format off
template <uint32_t Size>
using mo_mailbox_sender_t = mo_multi_producer_sequencer_t<mo_mail_t, Size>;
// clang-format on

// clang-format off
template <uint32_t Size>
using mo_mailbox_t = mo_ring_buffer_t<mo_mail_t, Size, mo_mailbox_sender_t<Size>, mo_mail_factory_t, mo_mail_translator_t>;
// clang-format on

template <uint32_t Size>
using mo_mailbox_receiver_t = mo_event_poller_t<mo_mailbox_t<Size>, mo_mailbox_sender_t<Size>, mo_mail_t>;

} // namespace mozi
