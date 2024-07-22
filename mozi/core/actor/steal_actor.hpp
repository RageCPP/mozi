#pragma once
#include "mozi/core/actor/poll_actor.hpp"
#include "mozi/core/alias.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <coroutine>
namespace mozi::actor
{
struct mo_steal_actor_data_s
{
    using mo__mailbox = mozi::mo_mailbox_t<BITS_2>;
    using mo__sender = mozi::mo_mailbox_sender_t<BITS_2>;
    using mo__reveiver = mozi::mo_mailbox_receiver_t<BITS_2>;
    using coro_handle = std::coroutine_handle<coro::mo_handle_s>;
    mo_steal_actor_data_s(coro_handle poll_actor_handle)
        : m_poll_actor_handle(poll_actor_handle),                   //
          m_state(actor::mo_actor_state_flags::MO_ACTOR_STATE_INIT) //
    {
        std::unique_ptr<mo__mailbox> mailbox = mo__mailbox::create_multi_producer();
        std::unique_ptr<mo__reveiver> mailbox_poller = mailbox->create_poller();
        m_mailbox_poller = std::move(mailbox_poller);
        m_mailbox = std::move(mailbox);
        spdlog::info("mo_steal_actor_data_s::mo_steal_actor_data_s()");
    }
    ~mo_steal_actor_data_s()
    {
        spdlog::info("mo_steal_actor_data_s::~mo_steal_actor_data_s()");
    }
    inline void stop() noexcept
    {
        m_state = actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    }
    inline bool is_stop() const noexcept
    {
        return m_state == actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    }
    inline void update_state(actor::mo_actor_state_flags state) noexcept
    {
        m_state = state;
    }
    inline actor::mo_actor_state_flags state() const noexcept
    {
        return m_state;
    }
    inline void poll(mo_mail_handle_t read) noexcept
    {
        m_mailbox_poller->poll(read);
    }
    inline void update_poll_actor_handle(coro_handle &handle) noexcept
    {
        m_poll_actor_handle = handle;
    }
    inline coro_handle poll_actor_handle() const noexcept
    {
        return m_poll_actor_handle;
    }
    inline void update_self_handle(coro_handle const &handle) noexcept
    {
        m_self_handle = handle;
    }
    inline void workflow_push_self_handle() noexcept
    {
        m_poll_actor_handle.promise().m_resource->write([handle = &m_self_handle](void *data) noexcept {
            mo_poll_actor_data_s *p_data = static_cast<mo_poll_actor_data_s *>(data);
            p_data->push_actor(*handle);
        });
    }
    inline bool send_message(mo_mail_translator_t &message) noexcept
    {
#ifndef NDEBUG
        spdlog::info("mailbox used capacity: {}", m_mailbox->used_capacity());
#endif
        auto is_success = m_mailbox->publish_event(message);
        auto used = m_mailbox->used_capacity();
        if (is_success && (used == 0 || used >= 5))
        {
            workflow_push_self_handle();
        }
        return is_success;
    }
    mo_steal_actor_data_s &operator=(const mo_steal_actor_data_s &) = delete;
    mo_steal_actor_data_s(const mo_steal_actor_data_s &) = delete;
    mo_steal_actor_data_s &operator=(mo_steal_actor_data_s &&) = delete;
    mo_steal_actor_data_s(mo_steal_actor_data_s &&) = delete;

  private:
    // Fixed order
    std::unique_ptr<mo__mailbox> m_mailbox;
    std::unique_ptr<mo__reveiver> m_mailbox_poller;
    // Fixed order
    coro_handle m_poll_actor_handle;
    coro_handle m_self_handle;
    actor::mo_actor_state_flags m_state;
};
inline void destroy_steal_actor_data(void *data) noexcept
{
    delete static_cast<mo_steal_actor_data_s *>(data);
}
} // namespace mozi::actor