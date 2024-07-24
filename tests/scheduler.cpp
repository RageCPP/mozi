#include "mozi/core/alias.hpp"
#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <mozi/core/scheduler/worker.hpp>

void read([[MO_UNUSED]] uint8_t *serial_in, //
          [[MO_UNUSED]] void *in_type,      //
          [[MO_UNUSED]] mozi::actor::mo_mail_out_s *mail_out) {};

TEST(MoziTest, SchedulerWorkerLifeCycle)
{
    spdlog::set_level(spdlog::level::debug);
    using namespace mozi;
    scheduler::mo_worker worker{};
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    auto run_result = worker.run_once();
    EXPECT_EQ(run_result, false);
    auto start_result = worker.start();
    EXPECT_EQ(start_result, true);
    run_result = worker.run_once();
    EXPECT_EQ(run_result, true);
    start_result = worker.start();
    EXPECT_EQ(start_result, false);
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    auto poll_actor_resource = worker.poll_actor_handle().promise().resource();
    actor::mo_poll_actor_data *poll_actor_data;
    poll_actor_resource->read(
        [&poll_actor_data](void *data) noexcept { poll_actor_data = static_cast<actor::mo_poll_actor_data *>(data); });
    EXPECT_EQ(poll_actor_data->mailbox_used_capacity(), 0);
    worker.stop();
    run_result = worker.run_once();
    EXPECT_EQ(run_result, false);
}
TEST(MoziTest, Mailbox)
{
    spdlog::set_level(spdlog::level::debug);
    using namespace mozi;
    scheduler::mo_worker worker{};
    auto start_result = worker.start();
    EXPECT_EQ(start_result, true);
    auto poll_actor_resource = worker.poll_actor_handle().promise().resource();
    actor::mo_poll_actor_data *poll_actor_data;
    poll_actor_resource->read(
        [&poll_actor_data](void *data) noexcept { poll_actor_data = static_cast<actor::mo_poll_actor_data *>(data); });

    auto steal_actor_future = actor::steal_actor_create(worker.poll_actor_handle());
    auto steal_actor_resource = steal_actor_future->resource();
    actor::mo_steal_actor_data *steal_actor_data;
    steal_actor_resource->write([&steal_actor_data]([[MO_UNUSED]] void *data) noexcept {
        steal_actor_data = static_cast<actor::mo_steal_actor_data *>(data);
    });
    mo_mail_translator_t translator{nullptr, nullptr, nullptr, read};
    steal_actor_data->send_mail(translator, *poll_actor_data);
    EXPECT_EQ(poll_actor_data->mailbox_used_capacity(), 1);
    worker.run_once();
    EXPECT_EQ(poll_actor_data->mailbox_used_capacity(), 0);
}