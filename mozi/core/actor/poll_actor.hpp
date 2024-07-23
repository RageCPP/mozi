#pragma once
#include "mozi/core/alias.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/deque.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <coroutine>
#include <cstdint>
#include <optional>
namespace mozi::actor
{
struct mo_poll_actor_data
{
    using mo__mailbox = mozi::mo_mailbox_t<BITS_2>;
    using mo__sender = mozi::mo_mailbox_sender_t<BITS_2>;
    using mo__reveiver = mozi::mo_mailbox_receiver_t<BITS_2>;
    using coro_handle = std::coroutine_handle<coro::mo_handle_s>;
    mo_poll_actor_data()
        : m_workflow{std::make_unique<mozi::mo_deque_c<coro_handle>>()},
          m_state(actor::mo_actor_state_flags::MO_ACTOR_STATE_INIT)
    {
        std::unique_ptr<mo__mailbox> mailbox = mo__mailbox::create_multi_producer();
        std::unique_ptr<mo__reveiver> mailbox_poller = mailbox->create_poller();
        m_mailbox_poller = std::move(mailbox_poller);
        m_mailbox = std::move(mailbox);
        spdlog::info("mo_poll_actor_data::mo_poll_actor_data()");
    }
    ~mo_poll_actor_data()
    {
        spdlog::info("mo_poll_actor_data::~mo_poll_actor_data()");
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
    inline std::optional<coro_handle> next_actor() noexcept
    {
        return m_workflow->pop();
    }
    inline void push_actor(coro_handle handle) noexcept
    {
        return m_workflow->push(handle);
    }
    inline void update_schedule_worker_handle(coro_handle &handle) noexcept
    {
        m_schedule_worker_handle = handle;
    }
    inline coro_handle schedule_worker_handle() const noexcept
    {
        return m_schedule_worker_handle;
    }
    inline uint32_t mailbox_used_capacity() const noexcept
    {
        return m_mailbox->used_capacity();
    }
    inline bool receive_mail(mo_mail_translator &mail) noexcept
    {
        return m_mailbox->publish_event(mail);
    }

    mo_poll_actor_data &operator=(const mo_poll_actor_data &) = delete;
    mo_poll_actor_data(const mo_poll_actor_data &) = delete;
    mo_poll_actor_data &operator=(mo_poll_actor_data &&) = delete;
    mo_poll_actor_data(mo_poll_actor_data &&) = delete;

  private:
    // Fixed order
    std::unique_ptr<mo__mailbox> m_mailbox;
    std::unique_ptr<mo__reveiver> m_mailbox_poller;
    // Fixed order
    std::unique_ptr<mozi::mo_deque_c<coro_handle>> m_workflow;
    coro_handle m_schedule_worker_handle;
    actor::mo_actor_state_flags m_state;
};

inline void destroy_poll_actor_data(void *info) noexcept
{
    delete static_cast<mo_poll_actor_data *>(info);
}
} // namespace mozi::actor