#include "mozi/core/actor_factory.hpp"
#include "mozi/db/redis.hpp"
#include <mozi/core/actor.hpp>
// #include <mozi/core/poll_actor.hpp>
#include <mozi/core/run.hpp>
#include <uv.h>

int main()
{
    // auto loop = std::make_unique<uv_loop_t>();
    // auto loop_raw = loop.release();
    // uv_loop_init(loop_raw);
    // mozi::redis::mo_redis_listen(loop_raw, 6379);
    // uv_run(loop_raw, UV_RUN_DEFAULT);

    // struct hello
    // {
    //     mozi::coro::mo_future_s *fut = nullptr;
    // };
    using namespace mozi;
    // using poll_data = mozi::actor::mo_poll_actor_data_s;
    std::unique_ptr<coro::mo_future_s> poll_actor = mozi::actor::poll_actor_create();
    spdlog::info("poll_actor->resume()");
    // std::unique_ptr<coro::mo_future_s> schedule_actor = mozi::actor::schedule_actor_create();

    // poll_actor->resume();
    // poll_actor->resource()->write([](void *data) noexcept {
    //     poll_data *p_data = static_cast<poll_data *>(data);
    //     p_data->stop();
    // });
    // poll_actor->resume();
    // poll_actor->destroy();
    return 0;
}