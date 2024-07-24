#pragma once
#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"

namespace mozi::coro
{
struct mo_poll_actor_awaiter
{
  public:
    mo_poll_actor_awaiter(mo_poll_c *resource) noexcept : m_resource(resource)
    {
    }

    bool await_ready() noexcept;

    std::coroutine_handle<mo_handle_s> await_suspend(std::coroutine_handle<mo_handle_s> h) noexcept;

    void await_resume() noexcept
    {
#ifndef NDEBUG
        spdlog::debug("poll actor resume read mail");
#endif
        using poll_actor_data_t = typename mozi::actor::mo_poll_actor_data;
        m_resource->read([](void *data) noexcept {
            poll_actor_data_t *p_data = static_cast<poll_actor_data_t *>(data);
            p_data->poll(mo_mail_handle_t{});
        });
    }

  private:
    mo_poll_c *m_resource = nullptr;
};
} // namespace mozi::coro