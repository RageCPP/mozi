#pragma once

// TODO: 窃取任务
#include "mozi/core/actor.hpp"
#include "mozi/core/deque.hpp"
namespace mozi::scheduler
{
inline void stealing([[maybe_unused]] mozi::mo_deque_c<mozi::actor::mo_actor_t::coro_handle> *workflow_1,
                     [[maybe_unused]] mozi::mo_deque_c<mozi::actor::mo_actor_t::coro_handle> *workflow_2)
{
    // TODO 窃取
}
} // namespace mozi::scheduler