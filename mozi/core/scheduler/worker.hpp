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
class mo_worker
{
    using coro_handle = std::coroutine_handle<coro::mo_handle>;

  public:
    enum class mo_worker_state_flags : uint8_t
    {
        MO_WORKER_STATE_INTT = 0x00,
        MO_WORKER_STATE_IDLE = 0x01,
        MO_WORKER_STATE_RUNNING = 0x02,
        MO_WORKER_STATE_STOP = 0x03,
    };
    using mo__state = mo_worker_state_flags;

    ~mo_worker() = default;
    mo_worker(const mo_worker &) = delete;
    mo_worker &operator=(const mo_worker &) = delete;
    mo_worker(mo_worker &&) = delete;
    mo_worker &operator=(mo_worker &&) = delete;
    mo_worker() noexcept : m_state(mo__state::MO_WORKER_STATE_INTT)
    {
        m_poll_actor = mozi::actor::poll_actor_create();
    }

    inline coro_handle poll_actor_handle() noexcept
    {
        return m_poll_actor->handle();
    }

    inline bool run_once() noexcept
    {
        // TODO: 仔细思考这里的内存屏障代码 memory_order_seq_cst 是否可优化为性能更高一点的
        auto expected = mo__state::MO_WORKER_STATE_IDLE;
        if (!m_state.compare_exchange_strong(expected, mo__state::MO_WORKER_STATE_RUNNING, std::memory_order_seq_cst))
        {
            return false;
        }
#ifndef NDEBUG
        spdlog::debug("========");
        spdlog::debug("mo_worker m_state {}", state_string());
#endif
        auto worker = run_once(coro::mo_coro_type_flags::MO_SCHEDULE_WORKER);
        worker.resume(); // initial_suspend resume
        m_state.store(expected, std::memory_order_seq_cst);
#ifndef NDEBUG
        spdlog::debug("mo_worker m_state {}", state_string());
        spdlog::debug("========");
#endif
        return true;
    }

    inline bool start() noexcept
    {
        auto expected = mo__state::MO_WORKER_STATE_INTT;
        if (!m_state.compare_exchange_strong(expected, mo__state::MO_WORKER_STATE_IDLE, std::memory_order_seq_cst))
        {
            return false;
        }
        run_once();
        return true;
    }

    inline bool stop() noexcept
    {
        // TODO: 需要完善在暂停失败时候如何处理
        auto expected = mo__state::MO_WORKER_STATE_IDLE;
        if (!m_state.compare_exchange_strong(expected, mo__state::MO_WORKER_STATE_STOP, std::memory_order_seq_cst))
        {
            spdlog::debug("faild stop mo_worker {}", state_string());
            return false;
        }
#ifndef NDEBUG
        spdlog::debug("========");
        spdlog::debug("mo_worker m_state {}", state_string());
#endif
        auto poll_actor_resource = m_poll_actor->resource();
        poll_actor_resource->write([](void *data) noexcept {
            mozi::actor::mo_poll_actor_data *p_data = static_cast<mozi::actor::mo_poll_actor_data *>(data);
            p_data->stop();
        });
        auto worker = run_once(coro::mo_coro_type_flags::MO_SCHEDULE_WORKER);
        worker.resume(); // initial_suspend resume
#ifndef NDEBUG
        spdlog::debug("========");
#endif
        return true;
    }

  private:
    std::string state_string() noexcept
    {
        switch (m_state)
        {
        case mo__state::MO_WORKER_STATE_INTT:
            return "MO_WORKER_STATE_INIT";
        case mo__state::MO_WORKER_STATE_IDLE:
            return "MO_WORKER_STATE_IDLE";
        case mo__state::MO_WORKER_STATE_RUNNING:
            return "MO_WORKER_STATE_RUNNING";
        case mo__state::MO_WORKER_STATE_STOP:
            return "MO_WORKER_STATE_STOP";
        default:
            return "MO_WORKER_STATE_UNKNOWN";
        }
    }
    [[MO_NODISCARD]] inline coro::mo_future run_once([[MO_UNUSED]] coro::mo_coro_type_flags flag) noexcept
    {
        co_await coro::mo_schedule_awaiter_transform_s{.fire = m_poll_actor.get()};
        // 这里逻辑为跳出本次协程执行循环等待下次调度 一定不要在这里回收 m_poll_actor 的资源
    }
    std::atomic<mo__state> m_state;
    std::unique_ptr<coro::mo_future> m_poll_actor;
};
} // namespace mozi::scheduler