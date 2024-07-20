#include "mozi/core/promise/handle.hpp"
#include "mozi/core/future.hpp"
namespace mozi::coro
{
mo_future_s mo_handle_s::get_return_object() noexcept
{
    return mo_future_s{mo_future_s::coro_handle::from_promise(*this)};
}
} // namespace mozi::coro