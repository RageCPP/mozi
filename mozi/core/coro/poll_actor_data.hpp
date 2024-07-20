#pragma once
#include "mozi/core/alias.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/deque.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <coroutine>
#include <optional>
namespace mozi::coro
{
struct mo_poll_actor_data_s
{
    using mo__mailbox = mozi::mo_mailbox_t<BITS_2>;
    using mo__sender = mozi::mo_mailbox_sender_t<BITS_2>;
    using mo__reveiver = mozi::mo_mailbox_receiver_t<BITS_2>;
    using coro_handle = std::coroutine_handle<mo_handle_s>;
    mo_poll_actor_data_s()
        : m_workflow{std::make_unique<mozi::mo_deque_c<coro_handle *>>()},
          m_state(actor::mo_actor_state_flags::MO_ACTOR_STATE_INIT)
    {
        std::unique_ptr<mo__mailbox> mailbox = mo__mailbox::create_multi_producer();
        std::unique_ptr<mo__reveiver> mailbox_poller = mailbox->create_poller();
        m_mailbox_poller = std::move(mailbox_poller);
        m_mailbox = std::move(mailbox);
        spdlog::info("mo__coro_s::mo__coro_s()");
    }
    ~mo_poll_actor_data_s()
    {
        spdlog::info("mo__coro_s::~mo__coro_s()");
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
    inline void poll([[maybe_unused]] mo_mail_read_t read) noexcept
    {
        m_mailbox_poller->poll(read);
    }
    inline std::optional<coro_handle *> next_actor() noexcept
    {
        return m_workflow->pop();
    }
    inline void push_actor(coro_handle *handle) noexcept
    {
        return m_workflow->push(handle);
    }
    // inline coro_handle schedule_handle() noexcept
    // {
    // return m_schedule_actor->handle();
    // }

    mo_poll_actor_data_s &operator=(const mo_poll_actor_data_s &) = delete;
    mo_poll_actor_data_s(const mo_poll_actor_data_s &) = delete;
    mo_poll_actor_data_s &operator=(mo_poll_actor_data_s &&) = delete;
    mo_poll_actor_data_s(mo_poll_actor_data_s &&) = delete;

  private:
    // Fixed order
    std::unique_ptr<mo__mailbox> m_mailbox;
    std::unique_ptr<mo__reveiver> m_mailbox_poller;
    // Fixed order
    std::unique_ptr<mozi::mo_deque_c<coro_handle *>> m_workflow;
    coro_handle m_schedule;
    actor::mo_actor_state_flags m_state;
};

inline void destroy_poll_actor_data(void *info) noexcept
{
    delete static_cast<mo_poll_actor_data_s *>(info);
}
} // namespace mozi::coro

//     struct mo__coro_s
//     {
//         struct mo__yield_awaiter
//         {
//             bool await_ready() noexcept
//             {
//                 return m_state == mo_actor_state_flags::MO_ACTOR_STATE_STOP;
//             }
//             std::coroutine_handle<> await_suspend(coro_handle handle) noexcept
//             {
//                 if (m_state == mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
//                 {
//                     handle.promise().m_mailbox_poller->poll(mo_mail_read_t{});
//                 }
//                 std::optional<std::coroutine_handle<>> h = handle.promise().m_workflow->pop();
//                 if (h.has_value())
//                 {
//                     return h.value();
//                 }
//                 else
//                 {
//                     return std::noop_coroutine();
//                 }
//             }
//             void await_resume() noexcept
//             {
//             }
//             mo_actor_state_flags m_state;

//           private:
//             friend struct mo__coro_s;
//             explicit mo__yield_awaiter(mo_actor_state_flags state) noexcept : m_state(state)
//             {
//             }
//         };

//         struct mo__out_state
//         {
//             mo_actor_state_flags m_state;
//         };
//         mo__yield_awaiter yield_value(mo__out_state &state) noexcept
//         {
//             state.m_state = m_state;
//             return mo__yield_awaiter{m_state};
//         }

//       private:
//         friend class mo_uv_actor_c;
//         // Fixed order
//         std::unique_ptr<mo__mailbox> m_mailbox;
//         std::unique_ptr<mo__reveiver> m_mailbox_poller;
//         // Fixed order

//         std::unique_ptr<mozi::mo_deque_c<std::coroutine_handle<>>> m_workflow;
//         mo_actor_state_flags m_state;
//     };

//     static mo_uv_actor_c create()
//     {
//         typename mo__coro_s::mo__out_state out_state{mo_actor_state_flags::MO_ACTOR_STATE_CHECK};
//         co_yield {out_state};

//         while (true)
//         {
//             co_yield {out_state};
//             if (out_state.m_state == mo_actor_state_flags::MO_ACTOR_STATE_STOP) [[MO_UNLIKELY]]
//             {
//                 break;
//             }
//         }
//         // TODO: 这里要不要增加一个一个永久挂起的节点 防止并发 resume
//     }
//     mo_uv_actor_c() noexcept = delete;
//     ~mo_uv_actor_c()
//     {
//         if (m_coro_handle.promise().m_state != mo_actor_state_flags::MO_ACTOR_STATE_STOP)
//         {
//             m_coro_handle.promise().update_state(mo_actor_state_flags::MO_ACTOR_STATE_STOP);
//             m_coro_handle.resume();
//         }
//         m_coro_handle.destroy();
//     }