#pragma once
#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/handle.hpp"

namespace mozi::coro
{
struct mo_schedule_awaiter_transform_s
{
    // poll actor
    mo_future_s *fire;
};
struct mo_schedule_awaiter_s
{
    using coro_handle = std::coroutine_handle<mo_handle_s>;

  public:
    bool await_ready() const noexcept
    {
        return false;
    }

    coro_handle await_suspend(coro_handle coro_handle) noexcept
    {
        using poll_actor_data_t = typename mozi::actor::mo_poll_actor_data_s;
        m_coro_handle.promise().m_resource->write([&coro_handle](void *data) noexcept {
            poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
            p_data->update_schedule_worker_handle(coro_handle);
        });
        return m_coro_handle;
    }

    void await_resume() noexcept
    {
    }

  private:
    friend struct mo_handle_s;
    explicit mo_schedule_awaiter_s(coro_handle coro_handle) noexcept : m_coro_handle(coro_handle)
    {
    }
    coro_handle m_coro_handle;
};
} // namespace mozi::coro