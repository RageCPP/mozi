#pragma once
#include "mozi/core/actor_flags.hpp"
#include "mozi/core/poll.hpp"
#include "spdlog/spdlog.h"
#include <coroutine>
#include <exception>
#include <memory>
namespace mozi::actor
{
struct mo_poll_actor_data_s;
}
namespace mozi::coro
{
struct mo_future_s
{
  public:
    struct mo__coro_s;
    using promise_type = mo__coro_s;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    struct mo__coro_s
    {
        struct mo__actor_awaiter;
        struct mo_actor_state;
        struct mo_poll_actor_state;
        struct mo__poll_actor_awaiter;
        // TODO 完善不同参数的构造函数
        mo__coro_s(std::shared_ptr<mo_poll_c> resource) noexcept : m_resource(resource)
        {
        }
        mo_future_s get_return_object() noexcept
        {
            return mo_future_s{coro_handle::from_promise(*this)};
        }
        suspend_never initial_suspend() noexcept
        {
            return {};
        }
        suspend_always final_suspend() noexcept
        {
            spdlog::info("mo__coro_s::final_suspend");
            return {};
        }
        void return_void() noexcept
        {
        }
        // TODO: return_value 在 协程执行结束后 虽然协程不能在恢复 但是信息被从 promise_type 中取出到 协程帧中
        void unhandled_exception() noexcept
        {
            // TODO 完善报错
            std::terminate();
        }

        mo__poll_actor_awaiter yield_value([[maybe_unused]] actor::mo_actor_state_flags info) noexcept;

        struct mo__poll_actor_awaiter
        {
          public:
            mo__poll_actor_awaiter(std::shared_ptr<mo_poll_c> resource) noexcept : m_resource(resource)
            {
            }
            bool await_ready() noexcept;

            void await_suspend([[maybe_unused]] coro_handle handle) noexcept
            {
                //     if (m_state == actor::mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
                //     {
                //         // handle.promise().m_value->m_mailbox_poller->poll(mo_mail_read_t{});
                //     }
                // return handle;
            }
            void await_resume() noexcept
            {
            }

          private:
            std::shared_ptr<mo_poll_c> m_resource = nullptr;
        };
        struct mo_poll_actor_state
        {
            actor::mo_actor_state_flags m_state;
        };

        struct mo__actor_awaiter
        {
            bool await_ready() noexcept
            {
                return m_state == actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
            }
            mo__actor_awaiter(actor::mo_actor_state_flags state) noexcept : m_state(state)
            {
            }

          private:
            actor::mo_actor_state_flags m_state;
        };
        struct mo_actor_state
        {
            actor::mo_actor_state_flags m_state;
        };

      private:
        friend struct mo_future_s;
        std::shared_ptr<mo_poll_c> m_resource = nullptr;
    };
    mo_future_s(const mo_future_s &) = delete;
    mo_future_s &operator=(const mo_future_s &) = delete;
    mo_future_s(mo_future_s &&ori) noexcept : m_coro_handle(std::exchange(ori.m_coro_handle, {}))
    {
        spdlog::info("mo_future_s::mo_future_s(mo_future_s &&)");
    }
    mo_future_s &operator=(mo_future_s &&) = delete;

    ~mo_future_s()
    {
        spdlog::info("mo_future_s::~mo_future_s");
    }

    inline void resume() noexcept
    {
        return m_coro_handle.resume();
    }
    inline void destroy() noexcept
    {
        return m_coro_handle.destroy();
    }
    inline coro_handle handle() noexcept
    {
        return m_coro_handle;
    }
    inline std::shared_ptr<mo_poll_c> resource() const noexcept
    {
        return m_coro_handle.promise().m_resource;
    }

  private:
    explicit mo_future_s(coro_handle handle) noexcept //
        : m_coro_handle(handle)                       //
    {
    }
    coro_handle m_coro_handle;
};
} // namespace mozi::coro