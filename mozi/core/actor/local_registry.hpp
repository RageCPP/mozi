#pragma once

#include "mozi/core/coro/future.hpp"
#include <cstdint>
#include <libcuckoo/cuckoohash_map.hh>

namespace mozi::actor
{
struct mo_local_actor_registry
{
  public:
    mo_local_actor_registry() noexcept
    {
    }
    mo_local_actor_registry(const mo_local_actor_registry &) = delete;
    mo_local_actor_registry &operator=(mo_local_actor_registry const &other) = delete;
    mo_local_actor_registry(mo_local_actor_registry &&ori) noexcept = delete;
    mo_local_actor_registry &operator=(mo_local_actor_registry &&ori) = delete;
    ~mo_local_actor_registry() = default;

    void insert(int64_t id, coro::mo_future &&actor) noexcept
    {
        m_local_actors.insert(id, std::move(actor));
    }

    coro::mo_future find(int64_t id) noexcept
    {
        return m_local_actors.find(id);
    }

    void erase(int64_t id) noexcept
    {
        m_local_actors.erase(id);
        // TODO: 如何处理 actor 销毁
        // 系统中可能有别的地方持有 actor 的引用 并且会向此 actor 发送消息
    }

  private:
    libcuckoo::cuckoohash_map<int64_t, coro::mo_future> m_local_actors = {};
};
} // namespace mozi::actor