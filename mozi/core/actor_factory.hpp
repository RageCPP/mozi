#pragma once
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/poll.hpp"
#include "mozi/core/coro/poll_actor_awaiter.hpp"
#include "mozi/core/coro/poll_actor_data.hpp"
#include <memory>
namespace mozi::actor
{
using mo_future_s = mozi::coro::mo_future_s;
inline mo_future_s poll_actor_create([[MO_UNUSED]] mo_coro_type_flags flag, [[MO_UNUSED]] coro::mo_poll_c *resource)
{
    co_yield {mo_actor_state_flags::MO_ACTOR_STATE_IDLE};
    while (true)
    {
        spdlog::info("mo_poll_actor_c::run once");
        co_yield {mo_actor_state_flags::MO_ACTOR_STATE_CHECK};
        bool is_stop = false;
        resource->read([&is_stop](void *data) noexcept {
            mozi::coro::mo_poll_actor_data_s *p_data = static_cast<mozi::coro::mo_poll_actor_data_s *>(data);
            is_stop = p_data->is_stop();
        });
        if (is_stop) [[MO_UNLIKELY]]
        {
            break;
        }
    }
    spdlog::info("mo_poll_actor_c::run end");
}
inline std::unique_ptr<mo_future_s> poll_actor_create()
{
    using mo_poll_c = coro::mo_poll_c;
    using mo_future_s = coro::mo_future_s;
    mo_poll_c *resource = new mo_poll_c{new mozi::coro::mo_poll_actor_data_s(), &coro::destroy_poll_actor_data};
    auto poll_actor = std::make_unique<mo_future_s>(poll_actor_create(mo_coro_type_flags::MO_POLL_ACTOR, resource));
    // poll_actor->resume();
    return poll_actor;
}

// inline mo_future_s schedule_actor_create([[MO_UNUSED]] mo_coro_type_flags flag, [[MO_UNUSED]] coro::mo_poll_c
// *resource)
// {
//     yield::schedule_symbol symbol{};
//     co_yield symbol;
//     spdlog::info("schedule_actor will be destroyed");
// }
// inline std::unique_ptr<mo_future_s> schedule_actor_create()
// {
//     using mo_poll_c = coro::mo_poll_c;
//     using mo_poll_actor_data_s = mozi::actor::mo_poll_actor_data_s;
//     constexpr auto MO_SCHEDULE_ACTOR = mo_coro_type_flags::MO_SCHEDULE_ACTOR;
//     mo_poll_c *resource = new mozi::coro::mo_poll_c(new mo_poll_actor_data_s(), nullptr);
//     auto schedule_actor = std::make_unique<mo_future_s>(schedule_actor_create(MO_SCHEDULE_ACTOR, resource));
//     return schedule_actor;
// }
} // namespace mozi::actor