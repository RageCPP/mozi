#pragma once

#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/actor/steal_actor.hpp"
#include "mozi/core/coro/flags.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
#include "mozi/core/coro/poll_actor_awaiter.hpp"
#include "mozi/core/coro/steal_actor_awaiter.hpp"

namespace mozi::actor
{
using mo_future_s = mozi::coro::mo_future_s;
using coro_handle = std::coroutine_handle<coro::mo_handle_s>;
using poll_actor_symbol_state = coro::yield_info::poll_actor_symbol_state;
using steal_actor_symbol_state = coro::yield_info::steal_actor_symbol_state;

inline mo_future_s poll_actor_create([[MO_UNUSED]] coro::mo_coro_type_flags flag,
                                     [[MO_UNUSED]] coro::mo_poll_c *resource) noexcept
{
    spdlog::info("poll_actor_create");
    co_yield poll_actor_symbol_state{mo_actor_state_flags::MO_ACTOR_STATE_IDLE};
    while (true)
    {
        spdlog::info("mo_poll_actor_c::run once");
        co_yield poll_actor_symbol_state{mo_actor_state_flags::MO_ACTOR_STATE_CHECK};
        bool is_stop = false;
        resource->read([&is_stop](void *data) noexcept {
            mozi::actor::mo_poll_actor_data *p_data = static_cast<mozi::actor::mo_poll_actor_data *>(data);
            is_stop = p_data->is_stop();
        });
        if (is_stop) [[MO_UNLIKELY]]
        {
            break;
        }
    }
    spdlog::info("mo_poll_actor_c::run end");
}
inline std::unique_ptr<mo_future_s> poll_actor_create() noexcept
{
    using mo_poll_c = coro::mo_poll_c;
    using mo_future_s = coro::mo_future_s;
    mo_poll_c *resource = new mo_poll_c{new mozi::actor::mo_poll_actor_data(), &actor::destroy_poll_actor_data};
    auto poll_actor =
        std::make_unique<mo_future_s>(poll_actor_create(coro::mo_coro_type_flags::MO_POLL_ACTOR, resource));
    return poll_actor;
}

inline mo_future_s steal_actor_create([[MO_UNUSED]] coro::mo_coro_type_flags flag,
                                      [[MO_UNUSED]] coro::mo_poll_c *resource) noexcept
{
    spdlog::info("steal_actor_create");
    co_yield steal_actor_symbol_state{mo_actor_state_flags::MO_ACTOR_STATE_IDLE};
    while (true)
    {
        spdlog::info("mo_steal_actor_c::run once");
        co_yield steal_actor_symbol_state{mo_actor_state_flags::MO_ACTOR_STATE_CHECK};
        bool is_stop = false;
        resource->read([&is_stop](void *data) noexcept {
            mozi::actor::mo_steal_actor_data_s *p_data = static_cast<mozi::actor::mo_steal_actor_data_s *>(data);
            is_stop = p_data->is_stop();
        });
        if (is_stop) [[MO_UNLIKELY]]
        {
            break;
        }
    }
    spdlog::info("mo_steal_actor_c::run end");
}
inline std::unique_ptr<mo_future_s> steal_actor_create(coro_handle poll_actor_handle) noexcept
{
    using mo_poll_c = coro::mo_poll_c;
    using mo_future_s = coro::mo_future_s;
    auto steal_actor_data = new mozi::actor::mo_steal_actor_data_s(poll_actor_handle);
    mo_poll_c *resource = new mo_poll_c{steal_actor_data, &actor::destroy_steal_actor_data};
    auto steal_actor =
        std::make_unique<mo_future_s>(steal_actor_create(coro::mo_coro_type_flags::MO_STEAL_ACTOR, resource));
    steal_actor_data->update_self_handle(steal_actor->handle());
    return steal_actor;
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
//     using mo_poll_actor_data = mozi::actor::mo_poll_actor_data;
//     constexpr auto MO_SCHEDULE_ACTOR = mo_coro_type_flags::MO_SCHEDULE_ACTOR;
//     mo_poll_c *resource = new mozi::coro::mo_poll_c(new mo_poll_actor_data(), nullptr);
//     auto schedule_actor = std::make_unique<mo_future_s>(schedule_actor_create(MO_SCHEDULE_ACTOR, resource));
//     return schedule_actor;
// }
} // namespace mozi::actor