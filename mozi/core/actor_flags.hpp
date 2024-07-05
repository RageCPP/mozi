#pragma once

namespace mozi::actor
{
enum class mo_actor_state_flags
{
    MO_ACTOR_STATE_INIT = 0x00,
    MO_ACTOR_STATE_IDLE = 0x01,
    MO_ACTOR_STATE_CHECK = 0x02,
    MO_ACTOR_STATE_BUSY = 0x03,
};
} // namespace mozi::actor