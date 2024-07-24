#pragma once
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"
#include "spdlog/spdlog.h"
#include <coroutine>

namespace mozi::coro
{
// TODO: 在debug 模式下给每个 future 都加个ID 观察回收测试
struct mo_future
{
  public:
    using promise_type = mo_handle;
    using coro_handle = std::coroutine_handle<promise_type>;
    using suspend_never = std::suspend_never;
    using suspend_always = std::suspend_always;

    mo_future(const mo_future &) = delete;
    mo_future &operator=(mo_future const &other)
    {
        if (this != &other)
        {
            m_coro_handle = other.m_coro_handle;
        }
        // m_coro_handle = std::exchange(ori.m_coro_handle, {});
        return *this;
    };
    mo_future(mo_future &&ori) noexcept : m_coro_handle(std::exchange(ori.m_coro_handle, {}))
    {
        spdlog::debug("mo_future::mo_future ori m_coro_handle is null: {}", ori.m_coro_handle.address() == nullptr);
        spdlog::debug("mo_future::mo_future(mo_future &&)");
    }
    mo_future &operator=(mo_future &&ori)
    {
        if (this != &ori)
        {
            m_coro_handle = std::exchange(ori.m_coro_handle, {});
        }
        return *this;
    }

    ~mo_future()
    {
        // spdlog::debug("mo_future::~mo_future");
        if (m_coro_handle.address() != nullptr)
        {
            // auto resource = m_coro_handle.promise().m_resource;
            // if (resource != nullptr)
            // {
            //     if (resource->m_destroy != nullptr)
            //     {
            //         resource->m_destroy(resource->m_data);
            //     }
            // }
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
    class mo_future {
    public:
        std::coroutine_handle<> m_coro_handle;

        std::coroutine_handle<>* handle_ptr() {
            return &m_coro_handle;
        }
    };

    mo_future original;
    auto ptr = original.handle_ptr();

    mo_future moved = std::move(original); // 移动操作
    // 此时 ptr 可能指向一个无效的位置
    */
    inline coro_handle handle_ptr() noexcept
    {
        return m_coro_handle;
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
    friend struct mo_handle;
    explicit mo_future(coro_handle handle) noexcept //
        : m_coro_handle(handle)                     //
    {
    }
    coro_handle m_coro_handle;
};
} // namespace mozi::coro