#pragma once
#include "mozi/core/future.hpp"
#include "mozi/core/poll_actor.hpp"
#include <memory>
namespace mozi::actor
{
inline static mozi::coro::mo_future_s create([[MO_UNUSED]] std::shared_ptr<coro::mo_poll_c> resource)
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
        if (is_stop)
        {
            break;
        }
        // if (state.m_state == mo_actor_state_flags::MO_ACTOR_STATE_STOP) [[MO_UNLIKELY]]
        // {
        //     break;
        // }
    }
    spdlog::info("mo_poll_actor_c::run end");
}
inline static std::unique_ptr<mozi::coro::mo_future_s> create()
{
    std::shared_ptr<mozi::coro::mo_poll_c> resource =
        std::make_shared<mozi::coro::mo_poll_c>(new mozi::actor::mo_poll_actor_data_s(), &destroy_poll_actor_data);
    auto poll_actor = std::make_unique<mozi::coro::mo_future_s>(create(resource));
    return poll_actor;
}
} // namespace mozi::actor