#pragma once

namespace mozi::coro
{
enum class mo_coro_type_flags
{
    MO_TASK = 0x00,
    MO_POLL_ACTOR = 0x01,
    MO_STEAL_ACTOR = 0x02,
    MO_SCHEDULE_WORKER = 0x03,
};
} // namespace mozi::coro