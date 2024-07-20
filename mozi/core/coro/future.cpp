#include "mozi/core/future.hpp"
#include "mozi/core/poll_actor.hpp"
#include "mozi/core/promise/handle.hpp"
namespace mozi::coro
{
using mo__coro_s = mo_handle_s;
using mo__poll_actor_awaiter = mo_handle_s::mo__poll_actor_awaiter;

mo__poll_actor_awaiter mo__coro_s::yield_value(actor::yield::poll_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        actor::mo_poll_actor_data_s *p_data = static_cast<typename actor::mo_poll_actor_data_s *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
};
} // namespace mozi::coro