#pragma once

#include "mozi/core/actor.hpp"
#include "mozi/core/deque.hpp"
#include <memory>
namespace mozi::scheduler
{
class mo_worker_c
{
  public:
    using mo__workflow = mozi::mo_deque_c<mozi::actor::mo_actor_t::coro_handle>;
    mo_worker_c() = default;
    ~mo_worker_c() = default;
    mo_worker_c(const mo_worker_c &) = delete;
    mo_worker_c &operator=(const mo_worker_c &) = delete;
    mo_worker_c(mo_worker_c &&) = delete;
    mo_worker_c &operator=(mo_worker_c &&) = delete;

    void run() noexcept
    {
    }
    mo__workflow *workflow() const noexcept
    {
        return m_workflow.get();
    }

  private:
    std::unique_ptr<mozi::actor::mo_actor_t> m_io_actor;
    std::unique_ptr<mo__workflow> m_workflow;
};
} // namespace mozi::scheduler