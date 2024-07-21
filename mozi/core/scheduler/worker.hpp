#pragma once
#include "mozi/core/actor/factory.hpp"
#include "mozi/core/coro/flags.hpp"
#include "mozi/core/coro/future.hpp"
#include "mozi/core/coro/schedule_awaiter.hpp"
#include "spdlog/spdlog.h"

namespace mozi::scheduler
{
class mo_worker_c
{
    using mo__future_t = mozi::coro::mo_future_s;

  public:
    enum class mo_worker_state_flags
    {
        MO_WORKER_STATE_IDLE = 0x00,
        MO_WORKER_STATE_RUNNING = 0x01,
    };
    ~mo_worker_c() = default;
    mo_worker_c(const mo_worker_c &) = delete;
    mo_worker_c &operator=(const mo_worker_c &) = delete;
    mo_worker_c(mo_worker_c &&) = delete;
    mo_worker_c &operator=(mo_worker_c &&) = delete;
    mo_worker_c() noexcept : m_state(mo_worker_state_flags::MO_WORKER_STATE_IDLE)
    {
        m_poll_actor = mozi::actor::poll_actor_create();
    }

    void run_once()
    {
#ifndef NDEBUG
        spdlog::info("   ");
        spdlog::info("mo_worker_c::run_once start");
#endif
        auto future = run_once(coro::mo_coro_type_flags::MO_SCHEDULE_WORKER);
        future.resume(); // initial_suspend resume

#ifndef NDEBUG
        spdlog::info("mo_worker_c::run_once end");
#endif

        auto is_done = future.done();
        spdlog::info("mo_worker_c::run_once done: {}", is_done);
    }

  private:
    [[MO_NODISCARD]] mo__future_t run_once([[MO_UNUSED]] coro::mo_coro_type_flags flag) noexcept
    {
        spdlog::info("mo_worker_c::run_once with flag start");
        co_await coro::mo_schedule_awaiter_transform_s{.fire = m_poll_actor.get()};
        spdlog::info("mo_worker_c::run_once with flag end");
        // 这里逻辑为跳出本次协程执行循环等待下次调度 一定不要在这里回收 m_poll_actor 的资源
    }
    std::atomic<mo_worker_state_flags> m_state;
    std::unique_ptr<mo__future_t> m_poll_actor;
};
} // namespace mozi::scheduler