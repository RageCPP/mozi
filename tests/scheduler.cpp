#include <gtest/gtest.h>
#include <mozi/core/scheduler/worker.hpp>

TEST(MoziTest, Scheduler)
{
    using namespace mozi;
    scheduler::mo_worker_c worker{};
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    worker.run_once();
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    auto worker_poll_actor = worker.poll_actor_handle().promise().resource();
    worker_poll_actor->read([](void *data) noexcept {
        auto p_data = static_cast<actor::mo_poll_actor_data *>(data);
        EXPECT_EQ(p_data->mailbox_used_capacity(), 0);
    });

    auto steal_actor_future = actor::steal_actor_create(worker.poll_actor_handle());
    auto steal_actor = steal_actor_future->resource();
    steal_actor->write([]([[MO_UNUSED]] void *data) noexcept {
        // auto p_data = static_cast<actor::mo_steal_actor_data_s *>(data);
    });
}