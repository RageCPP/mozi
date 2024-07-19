#include "mozi/core/future.hpp"
#include "mozi/core/poll_actor.hpp"
namespace mozi::coro
{
using mo__coro_s = mo_future_s::mo__coro_s;
using mo__poll_actor_awaiter = mo_future_s::mo__coro_s::mo__poll_actor_awaiter;

mo__poll_actor_awaiter mo__coro_s::yield_value(actor::yield::poll_actor_symbol_state &&info) noexcept
{
    m_resource->write([info = std::move(info)](void *data) noexcept {
        actor::mo_poll_actor_data_s *p_data = static_cast<typename actor::mo_poll_actor_data_s *>(data);
        p_data->update_state(info.m_state);
    });
    return {m_resource};
};

bool mo__poll_actor_awaiter::await_ready() noexcept
{
    bool is_ready = false;
    m_resource->read([&is_ready](void *data) noexcept {
        typename mozi::actor::mo_poll_actor_data_s *p_data =
            static_cast<typename mozi::actor::mo_poll_actor_data_s *>(data);
        is_ready = p_data->state() == actor::mo_actor_state_flags::MO_ACTOR_STATE_STOP;
    });
    return is_ready;
};

mo_future_s::coro_handle mo__poll_actor_awaiter::await_suspend([[MO_UNUSED]] mo_future_s::coro_handle h) noexcept
{
    using mo_poll_actor_data_s = typename mozi::actor::mo_poll_actor_data_s;
    // TODO: 未稳定

    actor::mo_actor_state_flags state;
    m_resource->read([&state](void *data) noexcept {
        mo_poll_actor_data_s *p_data = static_cast<mo_poll_actor_data_s *>(data);
        state = p_data->state();
    });
    if (state == actor::mo_actor_state_flags::MO_ACTOR_STATE_IDLE)
    {
        spdlog::info("mo_poll_actor_c::run idle");
        m_resource->read([]([[maybe_unused]] void *data) noexcept {
            mo_poll_actor_data_s *p_data = static_cast<mo_poll_actor_data_s *>(data);
            p_data->poll(mo_mail_read_t{});
        });
    }

    std::optional<mo_future_s::coro_handle *> handle = std::nullopt;
    m_resource->read([&handle](void *data) noexcept {
        mo_poll_actor_data_s *p_data = static_cast<mo_poll_actor_data_s *>(data);
        handle = p_data->next_actor();
    });
    if (handle.has_value())
    {
        return *(handle.value());
    }
    // else
    // {
    //     return h.promise().m_resource->read([](void *data) noexcept {
    //         mozi::actor::mo_poll_actor_data_s *p_data = static_cast<mozi::actor::mo_poll_actor_data_s *>(data);
    //         return p_data->schedule_handle();
    //     });
    // }
    return h;
};
} // namespace mozi::coro