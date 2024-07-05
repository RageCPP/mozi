#pragma once
#include "mozi/core/actor_flags.hpp"
#include "mozi/core/deque.hpp"
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <atomic>
#include <coroutine>
#include <cstdint>
#include <exception>
namespace mozi::actor
{
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator> class mo_actor_c
{
  public:
    struct mo__coro_s;
    using promise_type = mo__coro_s;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    using poller = mozi::ring::mo_ring_buffer_t<Event, Size, Sequencer, EventFactory, Translator>;
    struct mo_actor_info_s
    {
        mo_actor_state_flags m_state;
        poller::mo_event_poller_t *m_mailbox_poller;
        mozi::mo_deque_c<coro_handle> *m_workflows;
        mo_actor_c *m_self;
    };
    struct mo__coro_s
    {
        mo_actor_c get_return_object() noexcept
        {
            return mo_actor_c{coro_handle::from_promise(*this)};
        }
        suspend_never initial_suspend() noexcept
        {
            return {};
        }
        suspend_always final_suspend() noexcept
        {
            return {};
        }
        void return_void() noexcept
        {
        }
        void unhandled_exception() noexcept
        {
            // TODO 修复
            std::terminate();
        }

        struct mo__actor_awaiter
        {
            bool await_ready() noexcept
            {
                return false;
            }
            std::coroutine_handle<> await_suspend(coro_handle handle) noexcept
            {
                // if (m_state != mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
                // {
                // }
                // TODO: 时间片轮转
                // TODO: 循环处理信箱信件

                // handle.promise().get_return_object().m_mailbox;

                if (m_workflows->empty())
                {
                    return std::noop_coroutine();
                }
                // TODO 切换到下一个actor
                return handle;
            }
            void await_resume() noexcept
            {
            }

          private:
            friend struct mo__coro_s;
            explicit mo__actor_awaiter(mo_actor_state_flags flag,         //
                                       poller::mo_event_poller_t *poller, //
                                       mozi::mo_deque_c<coro_handle> *m_workflows) noexcept
                : m_state(flag), m_mailbox_poller(poller), m_workflows(m_workflows)
            {
            }
            mo_actor_state_flags m_state;
            poller::mo_event_poller_t *m_mailbox_poller;
            mozi::mo_deque_c<coro_handle> *m_workflows;
        };
        auto yield_value([[maybe_unused]] mo_actor_state_flags flag) noexcept -> decltype(auto)
        {
            // if (flag == mo_actor_state_flags::MO_ACTOR_STATE_CHECK)
            // {
            return mo__actor_awaiter{m_state.load(), m_mailbox_poller, m_workflows};
            // }
        }
        // TODO: 这里要修改 根据不同的状态有不同的挂起
        std::suspend_always yield_value(mo_actor_info_s info) noexcept
        {
            m_state.store(info.m_state);
            return {};
        }

        std::atomic<mo_actor_state_flags> m_state = mo_actor_state_flags::MO_ACTOR_STATE_INIT;
        poller::mo_event_poller_t *m_mailbox_poller;
        mozi::mo_deque_c<coro_handle> *m_workflows;
        mo_actor_c *m_self;
    };
    static mo_actor_c create() noexcept
    {
        co_yield mo_actor_info_s{
            .m_state = mo_actor_state_flags::MO_ACTOR_STATE_INIT,
        };

        while (true)
        {
            co_yield mo_actor_state_flags::MO_ACTOR_STATE_CHECK;
        }
    }
    mo_actor_state_flags state() const noexcept
    {
        return m_handle.promise().m_state.load();
    }

  private:
    mo_actor_c(coro_handle handle) noexcept : m_handle(handle)
    {
    }

    coro_handle m_handle;
    mozi::ring::mo_multi_producer_sequencer_t<Event, Size> m_mailbox;
};
// TODO: 这里要改为 多生产者
using mo_actor_t = mo_actor_c<mozi::mail::mo_mail_t,                                             //
                              4096,                                                              //
                              mozi::ring::mo_single_producer_sequencer_t<mozi::mail::mo_mail_t>, //
                              mozi::mail::mo_mail_factory_t,                                     //
                              mozi::mail::mo_mail_translator_t>;                                 //
} // namespace mozi::actor