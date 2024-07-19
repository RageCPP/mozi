#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/actor_flags.hpp"
#include "mozi/core/poll.hpp"
#include "spdlog/spdlog.h"
#include <coroutine>
#include <exception>
namespace mozi::actor
{
enum class mo_coro_type_flags
{
    MO_TASK = 0x00,
    MO_MOVEABLE_ACTOR = 0x01,
    MO_POLL_ACTOR = 0x02,
    MO_SCHEDULE_ACTOR = 0x03,
};
namespace yield
{
struct task_symbol
{
};
struct schedule_symbol
{
};
struct poll_actor_symbol
{
};
struct poll_actor_symbol_state
{
    actor::mo_actor_state_flags m_state;
};
struct moveable_actor_symbol
{
};
}; // namespace yield

struct mo_suspend_schedule_symbol
{
};
struct mo_poll_actor_data_s;
} // namespace mozi::actor
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
        struct mo_actor_state;
        struct mo__actor_awaiter;
        struct mo_poll_actor_state;
        struct mo__poll_actor_awaiter;
        struct mo__schedule_actor_awaiter;
        // TODO 完善不同参数的构造函数
        mo__coro_s(actor::mo_coro_type_flags flag, mo_poll_c *resource) noexcept
            : m_resource(resource), //
              m_flag(flag)          //
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
            if (m_flag == actor::mo_coro_type_flags::MO_SCHEDULE_ACTOR) [[MO_UNLIKELY]]
            {
            }
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

        mo__poll_actor_awaiter yield_value(actor::yield::poll_actor_symbol_state &&info) noexcept;

        mo__schedule_actor_awaiter yield_value([[maybe_unused]] actor::yield::schedule_symbol info) noexcept
        {
            return {};
        }

        struct mo__poll_actor_awaiter
        {
          public:
            mo__poll_actor_awaiter(mo_poll_c *resource) noexcept : m_resource(resource)
            {
            }

            bool await_ready() noexcept;

            coro_handle await_suspend(coro_handle h) noexcept;

            void await_resume() noexcept
            {
            }

          private:
            mo_poll_c *m_resource = nullptr;
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

        struct mo__schedule_actor_awaiter
        {
          public:
            bool await_ready() noexcept
            {
                return false;
            }
            void await_suspend([[MO_UNUSED]] coro_handle h) noexcept
            {
            }
            void await_resume() noexcept
            {
            }
        };

      private:
        friend struct mo_future_s;
        // TODO: 由于 coroutine_handle 的是 trival copyable，所以需要测试当 coroutine_handle 被复制时，作为
        // 共享资源的 m_resource 计数是否会增加，当 coroutine_handle 被销毁时，作为共享资源的 m_resource 是否会减少

        // TODO: 思考是否需要将 m_resource 改为普通指针 由框架来管理资源的生命周期
        mo_poll_c *m_resource = nullptr;
        actor::mo_coro_type_flags m_flag;
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
    /**
    class mo_future_s {
    public:
        std::coroutine_handle<> m_coro_handle;

        std::coroutine_handle<>* handle_ptr() {
            return &m_coro_handle;
        }
    };

    mo_future_s original;
    auto ptr = original.handle_ptr();

    mo_future_s moved = std::move(original); // 移动操作
    // 此时 ptr 可能指向一个无效的位置
    */
    inline coro_handle *handle_ptr() noexcept
    {
        return &m_coro_handle;
    }
    inline mo_poll_c *resource() const noexcept
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