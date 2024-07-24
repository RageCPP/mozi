#pragma once
#include "mozi/core/actor/factory.hpp"
#include "mozi/core/coro/flags.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/schedule_awaiter.hpp"
#include "spdlog/spdlog.h"
#include <atomic>

namespace mozi::scheduler
{
// TODO: lifetime
class mo_worker_c
{
    using mo__future_t = mozi::coro::mo_future;
    using coro_handle = std::coroutine_handle<coro::mo_handle_s>;

  public:
    enum class mo_worker_state_flags
    {
        MO_WORKER_STATE_INIT = 0x00,
        MO_WORKER_STATE_IDLE = 0x00,
        MO_WORKER_STATE_RUNNING = 0x01,
        MO_WORKER_STATE_STOP = 0x02,
    };
    static constexpr auto INIT_STATE = mo_worker_state_flags::MO_WORKER_STATE_INIT;
    static constexpr auto IDLE_STATE = mo_worker_state_flags::MO_WORKER_STATE_IDLE;
    static constexpr auto RUNNING_STATE = mo_worker_state_flags::MO_WORKER_STATE_RUNNING;
    static constexpr auto STOP_STATE = mo_worker_state_flags::MO_WORKER_STATE_STOP;

    ~mo_worker_c() = default;
    mo_worker_c(const mo_worker_c &) = delete;
    mo_worker_c &operator=(const mo_worker_c &) = delete;
    mo_worker_c(mo_worker_c &&) = delete;
    mo_worker_c &operator=(mo_worker_c &&) = delete;
    mo_worker_c() noexcept : m_state(mo_worker_state_flags::MO_WORKER_STATE_IDLE)
    {
        m_poll_actor = mozi::actor::poll_actor_create();
    }

    inline coro_handle poll_actor_handle() noexcept
    {
        return m_poll_actor->handle();
    }

    inline void run_once() noexcept
    {
        if (m_state.load(std::memory_order_relaxed) != IDLE_STATE)
        {
            return;
        }
        // TODO: 仔细思考这里的内存屏障代码是否可优化为性能更高一点的
        m_state.store(mo_worker_state_flags::MO_WORKER_STATE_RUNNING, std::memory_order_seq_cst);
#ifndef NDEBUG
        spdlog::debug("   ");
        spdlog::debug("mo_worker_c::run_once start");
#endif
        auto worker = run_once(coro::mo_coro_type_flags::MO_SCHEDULE_WORKER);
        worker.resume(); // initial_suspend resume
        m_state.store(mo_worker_state_flags::MO_WORKER_STATE_IDLE, std::memory_order_seq_cst);
    }

    inline void start() noexcept
    {
        m_state.store(mo_worker_state_flags::MO_WORKER_STATE_IDLE, std::memory_order_seq_cst);
        run_once();
    }

  private:
    [[MO_NODISCARD]] inline mo__future_t run_once([[MO_UNUSED]] coro::mo_coro_type_flags flag) noexcept
    {
        co_await coro::mo_schedule_awaiter_transform_s{.fire = m_poll_actor.get()};
        // 这里逻辑为跳出本次协程执行循环等待下次调度 一定不要在这里回收 m_poll_actor 的资源
    }
    std::atomic<mo_worker_state_flags> m_state = mo_worker_state_flags::MO_WORKER_STATE_INIT;
    std::unique_ptr<mo__future_t> m_poll_actor;
};
} // namespace mozi::scheduler