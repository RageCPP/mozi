#pragma once
#include "mozi/core/future.hpp"
#include "mozi/core/poll_actor.hpp"
#include <memory>
namespace mozi::actor
{
inline mozi::coro::mo_future_s poll_actor_create([[MO_UNUSED]] mo_coro_type_flags flag,
                                                 [[MO_UNUSED]] std::shared_ptr<coro::mo_poll_c> resource)
{
    // coro::mo_future_s::mo__coro_s::mo_poll_actor_state state{};
    co_yield mo_actor_state_flags::MO_ACTOR_STATE_IDLE;
    while (true)
    {
        spdlog::info("mo_poll_actor_c::run once");
        co_yield mo_actor_state_flags::MO_ACTOR_STATE_CHECK;
        bool is_stop = false;
        resource->read([&is_stop](void *data) noexcept {
            mozi::actor::mo_poll_actor_data_s *p_data = static_cast<mozi::actor::mo_poll_actor_data_s *>(data);
            is_stop = p_data->is_stop();
        });
        if (is_stop) [[MO_UNLIKELY]]
        {
            break;
        }
    }
    spdlog::info("mo_poll_actor_c::run end");
}
inline std::unique_ptr<mozi::coro::mo_future_s> poll_actor_create()
{
    std::shared_ptr<mozi::coro::mo_poll_c> resource =
        std::make_shared<mozi::coro::mo_poll_c>(new mozi::actor::mo_poll_actor_data_s(), &destroy_poll_actor_data);
    auto poll_actor =
        std::make_unique<mozi::coro::mo_future_s>(poll_actor_create(mo_coro_type_flags::MO_POLL_ACTOR, resource));
    return poll_actor;
}

inline mozi::coro::mo_future_s schedule_actor_create([[MO_UNUSED]] mo_coro_type_flags flag,
                                                     [[MO_UNUSED]] std::shared_ptr<coro::mo_poll_c> resource)
{
    yield::schedule_symbol symbol{};
    co_yield symbol;
    spdlog::info("schedule_actor will be destroyed");
}
inline std::unique_ptr<mozi::coro::mo_future_s> schedule_actor_create()
{
    std::shared_ptr<mozi::coro::mo_poll_c> resource =
        std::make_shared<mozi::coro::mo_poll_c>(new mozi::actor::mo_poll_actor_data_s(), nullptr);
    auto schedule_actor = std::make_unique<mozi::coro::mo_future_s>(
        schedule_actor_create(mo_coro_type_flags::MO_SCHEDULE_ACTOR, resource));
    return schedule_actor;
}
} // namespace mozi::actor