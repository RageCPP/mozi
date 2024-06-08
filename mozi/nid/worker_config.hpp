#pragma once
#include <cstdint>

namespace mozi::nid
{
struct mo_nid_worker_config_s
{
    /// > 1970-01-01 00:00:00, unit: ms
    int64_t base_time;
    /// [0, 15]
    uint32_t worker_id;
};
using mo_nid_worker_config_t = mo_nid_worker_config_s;
} // namespace mozi::nid