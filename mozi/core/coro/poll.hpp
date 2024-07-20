#pragma once

#include "spdlog/spdlog.h"
#include <type_traits>
namespace mozi::coro
{
struct mo_future_s;
struct mo_poll_c
{
    bool is_pedding() const noexcept
    {
        return m_flags == mo_poll_flags::MO_POLL_Pending;
    };
    bool is_ready() const noexcept
    {
        return m_flags == mo_poll_flags::MO_POLL_Ready;
    };
    mo_poll_c(void *data,                           //
              void (*destroy)(void *data) noexcept) //
        : m_data(data),                             //
          m_destroy(destroy)                        //
    {
    }
    ~mo_poll_c()
    {
        spdlog::info("mo_poll_c::~mo_poll_c()");
        if (m_destroy != nullptr)
        {
            m_destroy(m_data);
        }
    }

    template <typename F>
        requires std::invocable<const F &&, void *> && std::is_nothrow_invocable_v<const F &&, void *>
    inline void write(F write) noexcept
    {
        write(m_data);
    }

    // TODO 不要使用引用 不知道会不会阻止尾递归优化 后期测试检查生成的汇编代码
    template <typename F>
        requires std::invocable<const F &&, void *> && std::is_nothrow_invocable_v<const F &&, void *>
    inline auto read(F read) const noexcept -> std::invoke_result_t<const F &&, void *>
    {
        return read(m_data);
    }

    mo_poll_c(const mo_poll_c &) = delete;
    mo_poll_c &operator=(const mo_poll_c &) = delete;
    mo_poll_c(mo_poll_c &&) = delete;
    mo_poll_c &operator=(mo_poll_c &&) = delete;

  private:
    friend struct mo_future_s;
    enum class mo_poll_flags
    {
        MO_POLL_Pending,
        MO_POLL_Ready,
    };
    void *m_data;
    void (*m_destroy)(void *data) noexcept;
    mo_poll_flags m_flags = mo_poll_flags::MO_POLL_Pending;
};
} // namespace mozi::coro