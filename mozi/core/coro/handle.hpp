#pragma once
#include "mozi/compile/attributes_cpp.hpp"
#include "mozi/core/coro/flags.hpp"
#include "mozi/core/coro/poll.hpp"
#include "mozi/core/coro/yield_info.hpp"
#include <coroutine>

namespace mozi::scheduler
{
class mo_worker_c;
}
namespace mozi::actor
{
struct mo_steal_actor_data_s;
}
namespace mozi::coro
{
struct mo_future_s;
struct mo_poll_actor_awaiter_s;

struct mo_schedule_awaiter_s;
struct mo_schedule_awaiter_transform_s;

struct mo_steal_actor_awaiter_s;

struct mo_handle_s
{
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;
    using coro_handle = std::coroutine_handle<mo_handle_s>;

    explicit mo_handle_s([[MO_UNUSED]] scheduler::mo_worker_c &workder, mo_coro_type_flags flag) noexcept : m_flag(flag)
    {
    }
    explicit mo_handle_s(mo_coro_type_flags flag, mo_poll_c *resource) noexcept
        : m_resource(resource), //
          m_flag(flag)          //
    {
    }

    mo_future_s get_return_object() noexcept;

    suspend_always initial_suspend() noexcept
    {
        spdlog::info("mo_handle_s::initial_suspend");
        return {};
    }

    suspend_always final_suspend() noexcept
    {
        if (m_flag == mo_coro_type_flags::MO_SCHEDULE_WORKER) [[MO_UNLIKELY]]
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

    mo_poll_c *resource() const noexcept
    {
        return m_resource;
    }

    mo_poll_actor_awaiter_s yield_value(coro::yield_info::poll_actor_symbol_state &&info) noexcept;
    mo_steal_actor_awaiter_s yield_value(coro::yield_info::steal_actor_symbol_state &&info) noexcept;
    mo_schedule_awaiter_s await_transform(mo_schedule_awaiter_transform_s &&info) noexcept;

    // mo__schedule_actor_awaiter yield_value([[maybe_unused]] coro::yield_info::schedule_symbol info) noexcept
    // {
    //     return {};
    // }

  private:
    friend struct mo_future_s;
    friend struct mo_schedule_awaiter_s;
    friend struct actor::mo_steal_actor_data_s;
    // TODO: 由于 coroutine_handle 的是 trival copyable，所以需要测试当 coroutine_handle 被复制时，作为
    // 共享资源的 m_resource 计数是否会增加，当 coroutine_handle 被销毁时，作为共享资源的 m_resource 是否会减少

    // TODO: 思考是否需要将 m_resource 改为普通指针 由框架来管理资源的生命周期
    mo_poll_c *m_resource = nullptr;
    // 此参数不应该被修改
    mo_coro_type_flags m_flag;
};
} // namespace mozi::coro