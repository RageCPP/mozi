#pragma once
// #include "mozi/core/actor_flags.hpp"
// #include "mozi/core/alias.hpp"
// #include "spdlog/spdlog.h"
// #include <coroutine>
// #include <cstdint>
// #include <exception>
// #include <memory>
namespace mozi::actor
{

// template <uint32_t Size> class mo_actor_c
// {
//     using mo__mailbox = mo_mailbox_t<Size>;
//     using mo__sender = mo_mailbox_sender_t<Size>;
//     using mo__reveiver = mo_mailbox_receiver_t<Size>;

//   public:
//     struct mo__coro_s;
//     using promise_type = mo__coro_s;
//     using coro_handle = std::coroutine_handle<promise_type>;
//     using suspend_never = std::suspend_never;
//     using suspend_always = std::suspend_always;
//     struct mo__coro_s
//     {
//         mo_actor_c get_return_object() noexcept
//         {
//             return mo_actor_c{coro_handle::from_promise(*this)};
//         }
//         mo__coro_s() : m_state(mo_actor_state_flags::MO_ACTOR_STATE_INIT)
//         {
//             std::unique_ptr<mo__mailbox> mailbox = mo__mailbox::create_multi_producer();
//             std::unique_ptr<mo__reveiver> mailbox_poller = mailbox->create_poller();
//             m_mailbox_poller = std::move(mailbox_poller);
//             m_mailbox = std::move(mailbox);
//             spdlog::info("mo__coro_s::mo__coro_s()");
//         }
//         void update_uv_actor(mo_uv_actor_t *actor) noexcept
//         {
//             m_uv_actor = actor;
//         }
//         void update_state(mo_actor_state_flags state) noexcept
//         {
//             m_state = state;
//         }
//         inline mo_actor_state_flags state() const noexcept
//         {
//             return m_state;
//         }
//         suspend_never initial_suspend() noexcept
//         {
//             return {};
//         }
//         suspend_always final_suspend() noexcept
//         {
//             return {};
//         }
//         void return_void() noexcept
//         {
//         }
//         void unhandled_exception() noexcept
//         {
//             // TODO 完善报错
//             std::terminate();
//         }

//         struct mo__yield_awaiter
//         {
//             bool await_ready() noexcept
//             {
//                 return m_state == mo_actor_state_flags::MO_ACTOR_STATE_STOP;
//             }
//             std::coroutine_handle<> await_suspend(coro_handle handle) noexcept
//             {
//                 // TODO: 检查是否消耗完 如果未消耗完重新入队
//                 if (m_state == mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
//                 {
//                     // TODO: 检查信件是否消耗完
//                     handle.promise().m_mailbox_poller->poll(mo_mail_read_t{});
//                 }
//                 // std::optional<coro_handle> h = handle.promise().m_workflow->pop();
//                 auto uv_actor = handle.promise().m_uv_actor.load();
//                 if (uv_actor != nullptr)
//                 {
//                     return uv_actor->handle();
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
//         friend class mo_actor_c;
//         // Fixed order
//         std::unique_ptr<mo__mailbox> m_mailbox;
//         std::unique_ptr<mo__reveiver> m_mailbox_poller;
//         // Fixed order

//         std::atomic<mo_uv_actor_t *> m_uv_actor = nullptr;
//         mo_actor_state_flags m_state;
//     };

//     inline void resume() noexcept
//     {
//         return m_coro_handle.resume();
//     }
//     inline coro_handle handle() noexcept
//     {
//         return m_coro_handle;
//     }
//     inline void start() noexcept
//     {
//         m_coro_handle.promise().update_state(mo_actor_state_flags::MO_ACTOR_STATE_IDLE);
//     }
//     inline void stop() noexcept
//     {
//         m_coro_handle.promise().update_state(mo_actor_state_flags::MO_ACTOR_STATE_STOP);
//     }
//     inline void update_uv_actor(mo_uv_actor_t &actor) noexcept
//     {
//         m_coro_handle.promise().update_uv_actor(&actor);
//     }
//     inline void publish_event(mo_mail_translator_t &translate) noexcept
//     {
//         m_coro_handle.promise().m_mailbox->publish_event(translate);
//         auto uv_actor = m_coro_handle.promise().m_uv_actor.load();
//         if (uv_actor != nullptr)
//         {
//             spdlog::info("workflow_push");
//             uv_actor->workflow_push(m_coro_handle);
//         }
//         return;
//     }
//     static mo_actor_c create() noexcept
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
//     }
//     mo_actor_c() noexcept = delete;
//     ~mo_actor_c()
//     {
//     }
//     mo_actor_c &operator=(const mo_actor_c &) = delete;
//     mo_actor_c &operator=(mo_actor_c &&) = delete;
//     mo_actor_c(const mo_actor_c &) = delete;
//     mo_actor_c(mo_actor_c &&) = delete;

//   private:
//     mo_actor_c(coro_handle handle) noexcept //
//         : m_coro_handle(handle)             //
//     {
//     }

//     coro_handle m_coro_handle;
// };
} // namespace mozi::actor