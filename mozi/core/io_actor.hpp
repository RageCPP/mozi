#pragma once

#include "mozi/core/actor_flags.hpp"
#include "mozi/core/deque.hpp"
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/multi_producer_sequencer.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include <coroutine>
#include <cstdint>
#include <memory>
namespace mozi::actor
{
template <typename Event, uint32_t Size, class Sequencer, class EventFactory, class Translator> class mo_io_actor_c
{
  public:
    struct mo__coro_s;
    using promise_type = mo__coro_s;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    using poller = mozi::ring::mo_ring_buffer_t<Event, Size, Sequencer, EventFactory, Translator>;
    struct mo__coro_s
    {
        mo_io_actor_c get_return_object() noexcept
        {
            return mo_io_actor_c{coro_handle::from_promise(*this)};
        }
        mo__coro_s(mozi::mo_deque_c<coro_handle> *workflow, mo_actor_state_flags state)
            : m_workflow(workflow), //
              m_state(state)
        {
        }
        void update_state(mo_actor_state_flags state) noexcept
        {
            m_state = state;
        }
        mo_actor_state_flags state() const noexcept
        {
            return m_state;
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
        };
        mo__actor_awaiter yeild_value() noexcept
        {
            return {};
        }

      private:
        mo_actor_state_flags m_state;
        mozi::mo_deque_c<coro_handle> *m_workflow;
        poller::mo_event_poller_t *m_mailbox_poller;
    };
    coro_handle get_coro_handle() noexcept
    {
        return m_coro_handle;
    }
    // static std::unique_ptr<mo_io_actor_c> create(mozi::mo_deque_c<coro_handle> *workflow, mo_actor_state_flags state)
    // {
    //     using mail = mozi::mail::mo_mail_t;
    //     using mail_factory = mozi::mail::mo_mail_factory_t;
    //     using mo_mail_translator = mozi::mail::mo_mail_translator_t;
    //     using producer = mozi::ring::mo_multi_producer_sequencer_t<typename Event, uint32_t Size>

    // auto actor = std::make_unique<mo_io_actor_c>(init_io_actor(workflow, state));

    // actor.get_coro_handle().promise().update_state(state);

    // return actor;
    // }

  private:
    static mo_io_actor_c init_io_actor([[MO_UNUSED]] mozi::mo_deque_c<coro_handle> *workflow,
                                       [[MO_UNUSED]] mo_actor_state_flags state) noexcept
    {
    }
    mo_io_actor_c(coro_handle handle) noexcept : m_coro_handle(handle)
    {
    }

    coro_handle m_coro_handle;
    mozi::ring::mo_multi_producer_sequencer_t<Event, Size> m_mailbox;
};
} // namespace mozi::actor