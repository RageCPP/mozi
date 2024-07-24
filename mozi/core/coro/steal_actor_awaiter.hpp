#pragma once
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
namespace mozi::coro
{
struct mo_steal_actor_awaiter_s
{
  public:
    mo_steal_actor_awaiter_s(mo_poll_c *resource) noexcept : m_resource(resource)
    {
    }

    bool await_ready() noexcept
    {
        return false;
    }

    std::coroutine_handle<mo_handle> await_suspend(std::coroutine_handle<mo_handle> h) noexcept;

    void await_resume() noexcept
    {
    }

  private:
    mo_poll_c *m_resource = nullptr;
};
} // namespace mozi::coro