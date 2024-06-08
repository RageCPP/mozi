#pragma once

#include "r3.h"
#include <libcuckoo/cuckoohash_map.hh>

namespace mozi::http
{

/** mozi */
using mo_responder_t = class mo_responder_c;
using mo_loop_data_t = struct mo_loop_data_s;

/** aliases for types in r3 node */
using mo_route_t = R3Node;

/** aliases for types in libcuckoo */
using mo_responder_map_t = libcuckoo::cuckoohash_map<int64_t, std::shared_ptr<mo_responder_t>>;

} // namespace mozi::http
