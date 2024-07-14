#pragma once
#include "mozi/core/actor_flags.hpp"
#include "mozi/core/alias.hpp"
#include "mozi/core/deque.hpp"
#include "mozi/core/future.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include "mozi/variables/const_dec.hpp"
#include <memory>
#include <optional>
// TODO 完善报错

namespace mozi
{
namespace actor
{
struct mo_poll_actor_data_s
{
    using mo__mailbox = mozi::mo_mailbox_t<BITS_2>;
    using mo__sender = mozi::mo_mailbox_sender_t<BITS_2>;
    using mo__reveiver = mozi::mo_mailbox_receiver_t<BITS_2>;
    mo_poll_actor_data_s()
        : m_workflow{std::make_unique<mozi::mo_deque_c<coro::mo_future_s::coro_handle *>>()},
          m_state(mo_actor_state_flags::MO_ACTOR_STATE_INIT)
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
        m_state = mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    }
    inline bool is_stop() const noexcept
    {
        return m_state == mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    }
    inline void update_state(mo_actor_state_flags state) noexcept
    {
        m_state = state;
    }
    inline mo_actor_state_flags state() const noexcept
    {
        return m_state;
    }
    inline void poll(mo_mail_read_t read) noexcept
    {
        m_mailbox_poller->poll(read);
    }
    inline std::optional<coro::mo_future_s::coro_handle *> next_actor() noexcept
    {
        return m_workflow->pop();
    }
    inline void push_actor(coro::mo_future_s::coro_handle *handle) noexcept
    {
        return m_workflow->push(handle);
    }
    inline coro::mo_future_s::coro_handle schedule_handle() noexcept
    {
        return m_schedule_actor->handle();
    }

    mo_poll_actor_data_s &operator=(const mo_poll_actor_data_s &) = delete;
    mo_poll_actor_data_s(const mo_poll_actor_data_s &) = delete;
    mo_poll_actor_data_s &operator=(mo_poll_actor_data_s &&) = delete;
    mo_poll_actor_data_s(mo_poll_actor_data_s &&) = delete;

  private:
    // Fixed order
    std::unique_ptr<mo__mailbox> m_mailbox;
    std::unique_ptr<mo__reveiver> m_mailbox_poller;
    // Fixed order
    std::unique_ptr<mozi::mo_deque_c<coro::mo_future_s::coro_handle *>> m_workflow;
    std::unique_ptr<mozi::coro::mo_future_s> m_schedule_actor = nullptr;
    mo_actor_state_flags m_state;
};
inline void destroy_poll_actor_data(void *info) noexcept
{
    delete static_cast<mo_poll_actor_data_s *>(info);
}
} // namespace actor
} // namespace mozi

//     struct mo__coro_s;
//     using promise_type = mo__coro_s;
//     using coro_handle = std::coroutine_handle<promise_type>;
//     using suspend_never = std::suspend_never;
//     using suspend_always = std::suspend_always;
//     struct mo__coro_s
//     {
//         mo_uv_actor_c get_return_object() noexcept
//         {
//             return mo_uv_actor_c{coro_handle::from_promise(*this)};
//         }
//         mo__coro_s()
//             : m_workflow{std::make_unique<mozi::mo_deque_c<std::coroutine_handle<>>>()},
//               m_state(mo_actor_state_flags::MO_ACTOR_STATE_INIT)
//         {
//             std::unique_ptr<mo__mailbox> mailbox = mo__mailbox::create_multi_producer();
//             std::unique_ptr<mo__reveiver> mailbox_poller = mailbox->create_poller();
//             m_mailbox_poller = std::move(mailbox_poller);
//             m_mailbox = std::move(mailbox);
//             spdlog::info("mo__coro_s::mo__coro_s()");
//         }

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

//     inline void workflow_push(std::coroutine_handle<> handle) noexcept
//     {
//         return m_coro_handle.promise().m_workflow->push(handle);
//     }
//     inline bool publish_event(mo_mail_translator_t &translate) noexcept
//     {
//         return m_coro_handle.promise().m_mailbox->publish_event(translate);
//     }
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
//     mo_uv_actor_c &operator=(const mo_uv_actor_c &) = delete;
//     mo_uv_actor_c(const mo_uv_actor_c &) = delete;
//     mo_uv_actor_c &operator=(mo_uv_actor_c &&) = delete;
//     mo_uv_actor_c(mo_uv_actor_c &&) = delete;

//   private:
//     mo_uv_actor_c(coro_handle handle) noexcept //
//         : m_coro_handle(handle)                //
//     {
//     }

//     coro_handle m_coro_handle;