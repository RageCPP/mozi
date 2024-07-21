// #pragma once
// #include "mozi/core/coro/future.hpp"
// namespace mozi::scheduler
// {
// class mo_worker_c
// {
//     using mo__future_t = mozi::coro::mo_future_s;

//   public:
//     ~mo_worker_c() = default;
//     mo_worker_c(const mo_worker_c &) = delete;
//     mo_worker_c &operator=(const mo_worker_c &) = delete;
//     mo_worker_c(mo_worker_c &&) = delete;
//     mo_worker_c &operator=(mo_worker_c &&) = delete;
//     // mo_worker_c() noexcept
//     // {
//     // }

//     // mo__future_t run_once() const noexcept
//     // {

//     // }

//   private:
//     mo__future_t m_poll_actor;
// };
// } // namespace mozi::scheduler