#pragma once
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/handle.hpp"
#include <utility>

namespace mozi::coro
{
struct mo_schedule_awaiter_transform_s
{
    // poll actor
    mo_future_s fire;
};
struct mo_schedule_awaiter_s
{
    using coro_handle = std::coroutine_handle<mo_handle_s>;

  public:
    bool await_ready() const noexcept
    {
        return false;
    }

    coro_handle await_suspend([[MO_UNUSED]] coro_handle coro_handle) noexcept
    {
        // TODO: Implement the logic to schedule the actor
        std::unreachable();
    }

    void await_resume() noexcept
    {
    }

  private:
    explicit mo_schedule_awaiter_s(coro_handle coro_handle) noexcept : m_coro_handle(coro_handle)
    {
    }
    coro_handle m_coro_handle;
};
} // namespace mozi::coro