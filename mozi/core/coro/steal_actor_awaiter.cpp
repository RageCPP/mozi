#include "mozi/core/coro/steal_actor_awaiter.hpp"
#include "mozi/core/actor/flags.hpp"
#include "mozi/core/actor/steal_actor.hpp"
#include "mozi/core/coro/handle.hpp"
#include "mozi/core/coro/poll.hpp"

namespace mozi::coro
{
using steal_actor_data_t = typename mozi::actor::mo_steal_actor_data_s;
using coro_handle = std::coroutine_handle<mo_handle>;

coro_handle mo_steal_actor_awaiter_s::await_suspend([[MO_UNUSED]] coro_handle h) noexcept
{
    actor::mo_actor_state_flags state;
    m_resource->read([&state](void *data) noexcept {
        steal_actor_data_t *p_data = static_cast<steal_actor_data_t *>(data);
        state = p_data->state();
    });

    if (state == actor::mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
    {
        m_resource->read([](void *data) noexcept {
            steal_actor_data_t *p_data = static_cast<steal_actor_data_t *>(data);
            p_data->poll(mo_mail_handle_t{});
        });
    }

    return m_resource->read([](void *data) noexcept {
        steal_actor_data_t *p_data = static_cast<steal_actor_data_t *>(data);
        return p_data->poll_actor_handle();
    });
}
} // namespace mozi::coro