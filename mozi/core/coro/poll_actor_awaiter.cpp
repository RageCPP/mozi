#include "mozi/core/coro/poll_actor_awaiter.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
#include <coroutine>
#include <utility>

namespace mozi::coro
{
using poll_actor_data_t = typename mozi::actor::mo_poll_actor_data;
using coro_handle = std::coroutine_handle<mo_handle_s>;

// TODO 这里await_ready 在暂停后返回true好像会进入死循环 无法跳出调度
bool mo_poll_actor_awaiter::await_ready() noexcept
{
    // TODO 修改
    return m_resource->read([](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        return p_data->state() == actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    });
};

coro_handle mo_poll_actor_awaiter::await_suspend([[MO_UNUSED]] coro_handle h) noexcept
{
    // TODO: 未稳定
    static_assert(std::is_trivially_destructible<std::optional<coro_handle>>::value,
                  "std::optional<coro_handle> must be trivially destructible");
    std::optional<coro_handle> handle = std::nullopt;
    m_resource->read([&handle](void *data) noexcept {
        poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
        handle = p_data->next_actor();
    });
    if (handle.has_value())
    {
#ifndef NDEBUG
        spdlog::debug("poll actor will suspend goto steal actor");
#endif
        return handle.value();
    }
    else
    {
#ifndef NDEBUG
        spdlog::debug("poll actor will suspend goto schedule worker");
#endif
        // TODO: 修复
        return m_resource->read([](void *data) noexcept {
            poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
            return p_data->schedule_worker_handle();
        });
    }
}
} // namespace mozi::coro