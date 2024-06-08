#pragma once
#include "mozi/nid/worker.hpp"
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
} // namespace mozi