#pragma once

#include "mozi/core/actor/flags.hpp"

namespace mozi::coro::yield_info
{
struct schedule_symbol
{
};
struct poll_actor_symbol
{
};
struct poll_actor_symbol_state
{
    mozi::actor::mo_actor_state_flags m_state;
};
struct steal_actor_symbol
{
};
struct steal_actor_symbol_state
{
    mozi::actor::mo_actor_state_flags m_state;
};
} // namespace mozi::coro::yield_info