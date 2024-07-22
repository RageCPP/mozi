// #include "mozi/core/actor/steal_actor.hpp"
// #include "mozi/core/coro/handle.hpp"
// namespace mozi::actor
// {
// inline void mo_steal_actor_data_s::workflow_push_self_handle() noexcept
// {
//     {
//         m_poll_actor_handle.promise().m_resource->write([handle = m_self_handle](void *data) noexcept {
//             mo_poll_actor_data_s *p_data = static_cast<mo_poll_actor_data_s *>(data);
//             p_data->push_actor(&handle);
//         });
//     }
// }
// } // namespace mozi::actor