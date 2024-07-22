#include "mozi/core/actor/factory.hpp"
#include "mozi/db/redis.hpp"
#include <mozi/core/scheduler/worker.hpp>
// #include <mozi/core/poll_actor.hpp>
#include <mozi/core/run.hpp>
#include <uv.h>
auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data,
            [[maybe_unused]] mozi::actor::mo_mail_out_s *mail_out) {};
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
    mozi::scheduler::mo_worker_c worker{};
    worker.run_once();
    worker.run_once();
    auto handle = worker.poll_actor_handle();
    auto handle_1 = handle;
    spdlog::info("coroutine handle is equal: {}", handle == handle_1);
    spdlog::info("coroutine handle size: {}", sizeof(handle));
    // auto steal_actor = mozi::actor::steal_actor_create(worker.poll_actor_handle());
    // steal_actor->resource()->write([steal_actor = steal_actor.get()](void *data) noexcept {
    //     mozi::actor::mo_steal_actor_data_s *p_data = static_cast<mozi::actor::mo_steal_actor_data_s *>(data);
    //     uint8_t *bytes = new uint8_t[1];
    //     mozi::mo_mail_translator_t pub_mail{bytes, nullptr, nullptr, f};
    //     p_data->send_message(pub_mail);
    // });

    // using poll_data = mozi::actor::mo_poll_actor_data_s;
    // std::unique_ptr<coro::mo_future_s> poll_actor = mozi::actor::poll_actor_create();
    // spdlog::info("poll_actor->resume()");
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