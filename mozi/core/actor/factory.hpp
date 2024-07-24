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
using mo_future = mozi::coro::mo_future;
using coro_handle = std::coroutine_handle<coro::mo_handle>;
using poll_actor_symbol_state = coro::yield_info::poll_actor_symbol_state;
using steal_actor_symbol_state = coro::yield_info::steal_actor_symbol_state;
constexpr auto MO_ACTOR_STATE_IDLE = mo_actor_state_flags::MO_ACTOR_STATE_IDLE;
constexpr auto MO_ACTOR_STATE_CHECK = mo_actor_state_flags::MO_ACTOR_STATE_CHECK;

inline mo_future poll_actor_create([[MO_UNUSED]] coro::mo_coro_type_flags flag,
                                   [[MO_UNUSED]] coro::mo_poll_c *resource) noexcept
{
#ifndef NDEBUG
    spdlog::debug("poll actor life start");
#endif
    while (true)
    {
        co_yield poll_actor_symbol_state{MO_ACTOR_STATE_CHECK};

        // TODO: read from resource
        auto is_stop = resource->read([](void *data) noexcept {
            mozi::actor::mo_poll_actor_data *p_data = static_cast<mozi::actor::mo_poll_actor_data *>(data);
            return p_data->is_stop();
        });
        if (is_stop) [[MO_UNLIKELY]]
        {
            break;
        }
    }
#ifndef NDEBUG
    spdlog::debug("poll actor life end");
#endif
}
inline std::unique_ptr<mo_future> poll_actor_create() noexcept
{
    using mo_poll_c = coro::mo_poll_c;
    using mo_future = coro::mo_future;
    mo_poll_c *resource = new mo_poll_c{new mozi::actor::mo_poll_actor_data(), actor::destroy_poll_actor_data};
    auto poll_actor = std::make_unique<mo_future>(poll_actor_create(coro::mo_coro_type_flags::MO_POLL_ACTOR, resource));
    return poll_actor;
}

inline mo_future steal_actor_create([[MO_UNUSED]] coro::mo_coro_type_flags flag,
                                    [[MO_UNUSED]] coro::mo_poll_c *resource) noexcept
{
    spdlog::debug("steal_actor_create");
    co_yield steal_actor_symbol_state{mo_actor_state_flags::MO_ACTOR_STATE_IDLE};
    while (true)
    {
#ifndef NDEBUG
        spdlog::debug("mo_steal_actor_c::run once");
#endif
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
    spdlog::debug("mo_steal_actor_c::run end");
}
inline std::unique_ptr<mo_future> steal_actor_create(coro_handle poll_actor_handle) noexcept
{
    using mo_poll_c = coro::mo_poll_c;
    using mo_future = coro::mo_future;
    auto steal_actor_data = new mozi::actor::mo_steal_actor_data_s(poll_actor_handle);
    mo_poll_c *resource = new mo_poll_c{steal_actor_data, &actor::destroy_steal_actor_data};
    auto steal_actor =
        std::make_unique<mo_future>(steal_actor_create(coro::mo_coro_type_flags::MO_STEAL_ACTOR, resource));
    steal_actor_data->update_self_handle(steal_actor->handle());
    return steal_actor;
}

// inline mo_future schedule_actor_create([[MO_UNUSED]] mo_coro_type_flags flag, [[MO_UNUSED]] coro::mo_poll_c
// *resource)
// {
//     yield::schedule_symbol symbol{};
//     co_yield symbol;
//     spdlog::debug("schedule_actor will be destroyed");
// }
// inline std::unique_ptr<mo_future> schedule_actor_create()
// {
//     using mo_poll_c = coro::mo_poll_c;
//     using mo_poll_actor_data = mozi::actor::mo_poll_actor_data;
//     constexpr auto MO_SCHEDULE_ACTOR = mo_coro_type_flags::MO_SCHEDULE_ACTOR;
//     mo_poll_c *resource = new mozi::coro::mo_poll_c(new mo_poll_actor_data(), nullptr);
//     auto schedule_actor = std::make_unique<mo_future>(schedule_actor_create(MO_SCHEDULE_ACTOR, resource));
//     return schedule_actor;
// }
} // namespace mozi::actor