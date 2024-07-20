#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/actor.hpp"
#include "mozi/core/actor/flags.hpp"
#include "mozi/core/coro/poll.hpp"
#include <coroutine>
namespace mozi::actor
{

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
struct mo_future_s;
struct mo_handle_s
{
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    using coro_handle = std::coroutine_handle<mo_handle_s>;
    struct mo_actor_state;
    struct mo__actor_awaiter;
    struct mo_poll_actor_state;
    struct mo__poll_actor_awaiter;
    struct mo__schedule_actor_awaiter;

    // TODO 完善不同参数的构造函数
    mo_handle_s(actor::mo_coro_type_flags flag, mo_poll_c *resource) noexcept
        : m_resource(resource), //
          m_flag(flag)          //
    {
    }

    mo_future_s get_return_object() noexcept;

    suspend_never initial_suspend() noexcept
    {
        return {};
    }

    suspend_always final_suspend() noexcept
    {
        if (m_flag == actor::mo_coro_type_flags::MO_SCHEDULE_ACTOR) [[MO_UNLIKELY]]
        {
        }
        spdlog::info("mo_handle_s::final_suspend");
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
    // 此参数不应该被修改
    actor::mo_coro_type_flags m_flag;
};
} // namespace mozi::coro