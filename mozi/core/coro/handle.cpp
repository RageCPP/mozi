#include "mozi/core/coro/handle.hpp"
#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/actor/steal_actor.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/poll_actor_awaiter.hpp"
#include "mozi/core/coro/schedule_awaiter.hpp"
#include "mozi/core/coro/steal_actor_awaiter.hpp"
#include "spdlog/spdlog.h"
#include <coroutine>
namespace mozi::coro
{
using poll_actor_data_t = typename mozi::actor::mo_poll_actor_data;

mo_future_s mo_handle_s::get_return_object() noexcept
{
    spdlog::info("mo_handle_s::get_return_object");
    return mo_future_s{std::coroutine_handle<mo_handle_s>::from_promise(*this)};
}

mo_poll_actor_awaiter_s mo_handle_s::yield_value(coro::yield_info::poll_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
}

mo_steal_actor_awaiter_s mo_handle_s::yield_value(coro::yield_info::steal_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        mozi::actor::mo_steal_actor_data_s *p_data = static_cast<mozi::actor::mo_steal_actor_data_s *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
}

mo_schedule_awaiter_s mo_handle_s::await_transform(mo_schedule_awaiter_transform_s &&transform) noexcept
{
    spdlog::info("await_transform to mo_schedule_awaiter_s");
    return mo_schedule_awaiter_s{transform.fire->handle()};
}
} // namespace mozi::coro