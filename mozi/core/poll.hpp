#pragma once

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
        : m_data(data), m_destroy(destroy)
    {
    }
    ~mo_poll_c()
    {
        if (m_destroy != nullptr)
        {
            m_destroy(m_data);
        }
    }
    // TODO 改为使用 invocable 约束
    template <typename F> inline void write(F write) noexcept
    {
        write(m_data);
    }
    // inline void write(void (*write)(void *data) noexcept) noexcept
    // {
    //     write(m_data);
    // }
    // TODO 改为使用 invocable 约束 并且限制约束参数
    template <typename F> inline auto read(F read) const noexcept
    {
        return read(m_data);
    }
    // inline void read(void (*read)(void *data) noexcept) const noexcept
    // {
    //     read(m_data);
    // }
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