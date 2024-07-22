#pragma once
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
#include "spdlog/spdlog.h"
#include <coroutine>

namespace mozi::coro
{
// TODO: 在debug 模式下给每个 future 都加个ID 观察回收测试
struct mo_future_s
{
  public:
    using promise_type = mo_handle_s;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;

    mo_future_s(const mo_future_s &) = delete;
    mo_future_s &operator=(mo_future_s const &other)
    {
        if (this != &other)
        {
            m_coro_handle = other.m_coro_handle;
        }
        // m_coro_handle = std::exchange(ori.m_coro_handle, {});
        return *this;
    };
    mo_future_s(mo_future_s &&ori) noexcept : m_coro_handle(std::exchange(ori.m_coro_handle, {}))
    {
        spdlog::info("mo_future_s::mo_future_s ori m_coro_handle is null: {}", ori.m_coro_handle.address() == nullptr);
        spdlog::info("mo_future_s::mo_future_s(mo_future_s &&)");
    }
    mo_future_s &operator=(mo_future_s &&ori)
    {
        if (this != &ori)
        {
            m_coro_handle = std::exchange(ori.m_coro_handle, {});
        }
        return *this;
    }

    ~mo_future_s()
    {
        spdlog::info("mo_future_s::~mo_future_s");
        if (m_coro_handle.address() != nullptr)
        {
            m_coro_handle.destroy();
        }
    }

    inline void resume() noexcept
    {
        return m_coro_handle.resume();
    }
    inline void destroy() noexcept
    {
        return m_coro_handle.destroy();
    }
    inline coro_handle handle() noexcept
    {
        return m_coro_handle;
    }
    /**
    class mo_future_s {
    public:
        std::coroutine_handle<> m_coro_handle;

        std::coroutine_handle<>* handle_ptr() {
            return &m_coro_handle;
        }
    };

    mo_future_s original;
    auto ptr = original.handle_ptr();

    mo_future_s moved = std::move(original); // 移动操作
    // 此时 ptr 可能指向一个无效的位置
    */
    inline coro_handle *handle_ptr() noexcept
    {
        return &m_coro_handle;
    }
    inline coro_handle handle() const noexcept
    {
        return m_coro_handle;
    }
    inline mo_poll_c *resource() const noexcept
    {
        return m_coro_handle.promise().m_resource;
    }
    inline bool done() const noexcept
    {
        return m_coro_handle.done();
    }

  private:
    friend struct mo_handle_s;
    explicit mo_future_s(coro_handle handle) noexcept //
        : m_coro_handle(handle)                       //
    {
    }
    coro_handle m_coro_handle;
};
} // namespace mozi::coro