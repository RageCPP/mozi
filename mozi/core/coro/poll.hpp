#pragma once

#include "spdlog/spdlog.h"
#include <type_traits>

namespace mozi::coro
{
struct mo_future;
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
    // TODO: 这里必须是平凡析构函数 需要在别的地方检查析构
    // ~mo_poll_c()
    // {
    //     spdlog::debug("mo_poll_c::~mo_poll_c()");
    //     if (m_destroy != nullptr)
    //     {
    //         m_destroy(m_data);
    //     }
    // }

    // TODO 参数改为右值
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
    friend struct mo_future;
    friend struct mo_handle;
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