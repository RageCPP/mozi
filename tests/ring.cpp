#include "mozi/core/alias.hpp"
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/event_poller.hpp"
#include <cstdint>
#include <gtest/gtest.h>
#include <mozi/core/ring/ring_buffer.hpp>
#include <spdlog/spdlog.h>

constexpr uint32_t mail_size = 512;
using mail = mozi::mo_mail_t;
using mail_factory = mozi::mo_mail_factory_t;
using mo_mail_translator = mozi::mo_mail_translator_t;
using producer = mozi::mo_multi_producer_sequencer_t<mail, mail_size>;
using ring_buffer = mozi::mo_ring_buffer_t<mail, mail_size, producer, mail_factory, mo_mail_translator>;
using poller = mozi::mo_event_poller_t<ring_buffer, producer, mail>;
void publish_with_5thread(auto &producer, uint16_t &success, uint16_t count)
{
    std::jthread([&producer, &success, count]() {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) {};
        auto num = 0;
        while (count > num)
        {
            uint8_t *bytes = new uint8_t[1];
            mo_mail_translator pub_mail{bytes, nullptr, f};
            auto is_sucess = producer->publish_event(pub_mail);
            if (is_sucess)
            {
                success += 1;
            }
            num += 1;
        }
    });
    std::jthread([&producer, &success, count]() {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) {};
        auto num = 0;
        while (count > num)
        {
            uint8_t *bytes = new uint8_t[1];
            mo_mail_translator pub_mail{bytes, nullptr, f};
            auto is_sucess = producer->publish_event(pub_mail);
            if (is_sucess)
            {
                success += 1;
            }
            num += 1;
        }
    });
    std::jthread([&producer, &success, count]() {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) {};
        auto num = 0;
        while (count > num)
        {
            uint8_t *bytes = new uint8_t[1];
            mo_mail_translator pub_mail{bytes, nullptr, f};
            auto is_sucess = producer->publish_event(pub_mail);
            if (is_sucess)
            {
                success += 1;
            }
            num += 1;
        }
    });
    std::jthread([&producer, &success, count]() {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) {};
        auto num = 0;
        while (count > num)
        {
            uint8_t *bytes = new uint8_t[1];
            mo_mail_translator pub_mail{bytes, nullptr, f};
            auto is_sucess = producer->publish_event(pub_mail);
            if (is_sucess)
            {
                success += 1;
            }
            num += 1;
        }
    });
    std::jthread([&producer, &success, count]() {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) {};
        auto num = 0;
        while (count > num)
        {
            uint8_t *bytes = new uint8_t[1];
            mo_mail_translator pub_mail{bytes, nullptr, f};
            auto is_sucess = producer->publish_event(pub_mail);
            if (is_sucess)
            {
                success += 1;
            }
            num += 1;
        }
    });
}

TEST(MoziTest, GattingSequence)
{
    auto multi_producer = ring_buffer::create_multi_producer();
    mozi::ring::mo_arc_sequence_t gating_sequence_1 = std::make_shared<mozi::ring::mo_sequence_t>();
    gating_sequence_1->set(SIZE_MAX - 2);
    multi_producer->claim(SIZE_MAX - 2);
    multi_producer->add_gating_sequences(gating_sequence_1);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 1);

    mozi::ring::mo_arc_sequence_t gating_sequence_2 = std::make_shared<mozi::ring::mo_sequence_t>();
    gating_sequence_2->set(SIZE_MAX - 2);
    multi_producer->claim(SIZE_MAX - 2);
    multi_producer->add_gating_sequences(gating_sequence_2);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 2);

    mozi::ring::mo_arc_sequence_t gating_sequence_3 = std::make_shared<mozi::ring::mo_sequence_t>();
    gating_sequence_3->set(SIZE_MAX - 2);
    multi_producer->claim(SIZE_MAX - 2);
    multi_producer->add_gating_sequences(gating_sequence_3);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 3);

    multi_producer->remove_gating_sequence(gating_sequence_1);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 2);
    multi_producer->remove_gating_sequence(gating_sequence_2);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 1);
    multi_producer->remove_gating_sequence(gating_sequence_3);
    EXPECT_EQ(multi_producer->gating_sequences_size(), 0);
}

TEST(MoziTest, Publish)
{

    auto multi_producer = ring_buffer::create_multi_producer();
    uint16_t success = 0;

    for (uint16_t i = 0; i < 100; i++)
    {
        EXPECT_EQ(success, 0);
        publish_with_5thread(multi_producer, success, 500);
        EXPECT_EQ(success, 2500);
        success = 0;
    }

    for (uint16_t i = 0; i < 100; i++)
    {
        EXPECT_EQ(success, 0);
        mozi::ring::mo_arc_sequence_t gating_sequence = std::make_shared<mozi::ring::mo_sequence_t>();
        multi_producer->add_gating_sequences(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 1);
        publish_with_5thread(multi_producer, success, 500);
        EXPECT_EQ(success, 512);
        success = 0;
        multi_producer->remove_gating_sequence(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 0);
    }

    for (uint16_t i = 0; i < 100; i++)
    {
        EXPECT_EQ(success, 0);
        mozi::ring::mo_arc_sequence_t gating_sequence = std::make_shared<mozi::ring::mo_sequence_t>();
        gating_sequence->set(SIZE_MAX - 2);
        multi_producer->claim(SIZE_MAX - 2);
        multi_producer->add_gating_sequences(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 1);
        publish_with_5thread(multi_producer, success, 500);
        EXPECT_EQ(success, 512);
        success = 0;
        multi_producer->remove_gating_sequence(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 0);
    }

    for (uint16_t i = 0; i < 100; i++)
    {
        EXPECT_EQ(success, 0);
        mozi::ring::mo_arc_sequence_t gating_sequence = std::make_shared<mozi::ring::mo_sequence_t>();
        gating_sequence->set(SIZE_MAX - 200);
        multi_producer->claim(SIZE_MAX - 200);
        multi_producer->add_gating_sequences(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 1);

        publish_with_5thread(multi_producer, success, 500);
        EXPECT_EQ(success, 512);
        success = 0;
        multi_producer->remove_gating_sequence(gating_sequence);
        EXPECT_EQ(multi_producer->gating_sequences_size(), 0);
    }

    for (uint16_t i = 0; i < 100; i++)
    {
        {
            auto poller = multi_producer->create_poller();
            EXPECT_EQ(multi_producer->gating_sequences_size(), 1);
            EXPECT_EQ(success, 0);
            publish_with_5thread(multi_producer, success, 500);
            EXPECT_EQ(success, 512);
            uint16_t second_success = 0;
            publish_with_5thread(multi_producer, second_success, 500);
            EXPECT_EQ(second_success, 0);
            second_success = 0;
            auto poll_state = poller->poll(mozi::mo_mail_read_t{});
            EXPECT_EQ(poll_state, poller::mo_poll_flags::MO_POLL_PROCESSING);
            poll_state = poller->poll(mozi::mo_mail_read_t{});
            EXPECT_EQ(poll_state, poller::mo_poll_flags::MO_POLL_IDLE);
            publish_with_5thread(multi_producer, second_success, 500);
            EXPECT_EQ(second_success, 512);
            second_success = 0;
            poll_state = poller->poll(mozi::mo_mail_read_t{});
            EXPECT_EQ(poll_state, poller::mo_poll_flags::MO_POLL_PROCESSING);
            publish_with_5thread(multi_producer, second_success, 1);
            EXPECT_EQ(second_success, 5);
            success = 0;
        }
        EXPECT_EQ(multi_producer->gating_sequences_size(), 0);
    }
}