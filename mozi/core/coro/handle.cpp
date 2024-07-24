#include "mozi/core/coro/handle.hpp"
#include "fmt/core.h"
#include "mozi/core/actor/flags.hpp"
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
using mo_poll_actor_data = typename mozi::actor::mo_poll_actor_data;

mo_future mo_handle::get_return_object() noexcept
{
    return mo_future{std::coroutine_handle<mo_handle>::from_promise(*this)};
}

mo_poll_actor_awaiter mo_handle::yield_value(coro::yield_info::poll_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        mo_poll_actor_data *p_data = static_cast<mo_poll_actor_data *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
}

mo_steal_actor_awaiter_s mo_handle::yield_value(coro::yield_info::steal_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        mozi::actor::mo_steal_actor_data *p_data = static_cast<mozi::actor::mo_steal_actor_data *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
}

mo_schedule_awaiter_s mo_handle::await_transform(mo_schedule_awaiter_transform_s &&transform) noexcept
{
    // fire->handle(): poll actor handle
    return mo_schedule_awaiter_s{transform.fire->handle()};
}
} // namespace mozi::coro