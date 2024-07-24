#pragma once
#include <string>
namespace mozi::actor
{
enum class mo_actor_state_flags
{
    MO_ACTOR_STATE_INIT = 0x00,
    MO_ACTOR_STATE_IDLE = 0x01,
    MO_ACTOR_STATE_CHECK = 0x02,
    MO_ACTOR_STATE_BUSY = 0x03,
    MO_ACTOR_STATE_STOP = 0x04,
};
inline std::string to_string(mo_actor_state_flags state)
{
    switch (state)
    {
    case mo_actor_state_flags::MO_ACTOR_STATE_INIT:
        return "MO_ACTOR_STATE_INIT";
    case mo_actor_state_flags::MO_ACTOR_STATE_IDLE:
        return "MO_ACTOR_STATE_IDLE";
    case mo_actor_state_flags::MO_ACTOR_STATE_CHECK:
        return "MO_ACTOR_STATE_CHECK";
    case mo_actor_state_flags::MO_ACTOR_STATE_BUSY:
        return "MO_ACTOR_STATE_BUSY";
    case mo_actor_state_flags::MO_ACTOR_STATE_STOP:
        return "MO_ACTOR_STATE_STOP";
    default:
        return "MO_ACTOR_STATE_UNKNOWN";
    }
}
} // namespace mozi::actor