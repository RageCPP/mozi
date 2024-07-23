#include "mozi/core/coro/poll_actor_awaiter.hpp"
#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
#include <coroutine>
#include <utility>

namespace mozi::coro
{
using poll_actor_data_t = typename mozi::actor::mo_poll_actor_data;
using coro_handle = std::coroutine_handle<mo_handle_s>;

// TODO 这里await_ready 在暂停后返回true好像会进入死循环 无法跳出调度
bool mo_poll_actor_awaiter_s::await_ready() noexcept
{
    return m_resource->read([](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        return p_data->state() == actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    });
};

coro_handle mo_poll_actor_awaiter_s::await_suspend([[MO_UNUSED]] coro_handle h) noexcept
{
    // TODO: 未稳定

    actor::mo_actor_state_flags state;
    m_resource->read([&state](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        state = p_data->state();
    });

    if (state == actor::mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
    {
        spdlog::info("mo_poll_actor_c::run idle");
        m_resource->read([](void *data) noexcept {
            poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
            p_data->poll(mo_mail_handle_t{});
        });
    }

    std::optional<coro_handle> handle = std::nullopt;
    m_resource->read([&handle](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        handle = p_data->next_actor();
    });
    if (handle.has_value())
    {
        return handle.value();
    }
    else
    {
        spdlog::info("mo_poll_actor_c::run end");
        // TODO: 修复
        return m_resource->read([](void *data) noexcept {
            poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
            return p_data->schedule_worker_handle();
        });
    }
}
} // namespace mozi::coro