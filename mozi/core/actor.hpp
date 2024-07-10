#pragma once
#include "mozi/core/actor_flags.hpp"
#include "mozi/core/alias.hpp"
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include "mozi/core/uv_actor.hpp"
#include <coroutine>
#include <cstdint>
namespace mozi::actor
{
template <uint32_t Size> class mo_actor_c
{
    using mo__mailbox = mo_mailbox_t<Size>;
    using mo__sender = mo_mailbox_sender_t<Size>;
    using mo__reveiver = mo_mailbox_receiver_t<Size>;

  public:
    struct mo__coro_s;
    using promise_type = mo__coro_s;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    struct mo__coro_s
    {
        mo_actor_c get_return_object() noexcept
        {
            return mo_actor_c{coro_handle::from_promise(*this)};
        }
        void update_uv_actor(mo_uv_actor_c<Size> *actor) noexcept
        {
            m_uv_actor = actor;
        }
        inline mo_actor_state_flags state() const noexcept
        {
        }

      private:
        mo_uv_actor_t *m_uv_actor = nullptr;
        mo_actor_state_flags m_state;
    };

    static mo_actor_c create() noexcept
    {
    }
    mo_actor_c() noexcept = delete;
    ~mo_actor_c()
    {
    }
    mo_actor_c &operator=(const mo_actor_c &) = delete;
    mo_actor_c &operator=(mo_actor_c &&) = delete;
    mo_actor_c(const mo_actor_c &) = delete;
    mo_actor_c(mo_actor_c &&) = delete;

  private:
    mo_actor_c(coro_handle handle) noexcept : m_coro_handle(handle)
    {
    }

    coro_handle m_coro_handle;
};
} // namespace mozi::actor