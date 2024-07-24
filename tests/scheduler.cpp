#include "mozi/core/alias.hpp"
#include "spdlog/spdlog.h"
#include <gtest/gtest.h>
#include <mozi/core/scheduler/worker.hpp>

void read([[MO_UNUSED]] uint8_t *serial_in, //
          [[MO_UNUSED]] void *in_type,      //
          [[MO_UNUSED]] mozi::actor::mo_mail_out_s *mail_out) {};

TEST(MoziTest, Scheduler)
{
    spdlog::set_level(spdlog::level::debug);
    using namespace mozi;
    scheduler::mo_worker_c worker{};
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    worker.run_once();
    EXPECT_EQ(worker.poll_actor_handle().done(), false);
    auto worker_poll_actor = worker.poll_actor_handle().promise().resource();
    actor::mo_poll_actor_data *poll_actor_data;
    worker_poll_actor->read([&poll_actor_data](void *data) noexcept {
        auto p_data = static_cast<actor::mo_poll_actor_data *>(data);
        poll_actor_data = p_data;
        EXPECT_EQ(p_data->mailbox_used_capacity(), 0);
    });

    auto steal_actor_future = actor::steal_actor_create(worker.poll_actor_handle());
    auto steal_actor = steal_actor_future->resource();
    steal_actor->write([&poll_actor_data]([[MO_UNUSED]] void *data) noexcept {
        auto p_data = static_cast<actor::mo_steal_actor_data_s *>(data);

        mo_mail_translator_t translator{nullptr, nullptr, nullptr, read};
        p_data->send_mail(translator, *poll_actor_data);
    });
    EXPECT_EQ(poll_actor_data->mailbox_used_capacity(), 1);
    worker.run_once();
    EXPECT_EQ(poll_actor_data->mailbox_used_capacity(), 0);
}