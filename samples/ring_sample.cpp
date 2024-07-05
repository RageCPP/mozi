#include "fmt/core.h"
#include "mozi/core/mail.hpp"
#include "mozi/core/ring/processing_sequence_barrier.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include "mozi/core/ring/single_producer_sequencer.hpp"
#include "mozi/core/ring/yield_wait_strategy.hpp"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <tuple>
#include <vector>
// int random_1_to_5()
// {
//     std::random_device rd;                         // 用于获取随机数种子
//     std::mt19937 gen(rd());                        // 使用Mersenne Twister算法生成随机数
//     std::uniform_int_distribution<> distrib(1, 5); // 定义一个均匀分布的随机数生成器，范围是1到5
//     return distrib(gen);
// }

// int main()
// {
//     std::vector<std::tuple<uint8_t, uint8_t>> list;
//     for (int i = 0; i < 10000; i++)
//     {
//         list.emplace_back(random_1_to_5(), random_1_to_5());
//     }
//     mozi::ring::mo_sequence_t sequence;

//     sequence.set(1);
//     auto start_if = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < 100; i++)
//     {
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             sequence.set(1);
//             sequence.compare_and_set(1, 2);
//             sequence.compare_and_set(2, 1);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             sequence.set(1);
//             sequence.compare_and_set(1, 2);
//             sequence.compare_and_set(2, 1);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//     }
//     auto end_if = std::chrono::high_resolution_clock::now();
//     auto if_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_if - start_if).count();
//     fmt::println("if 10000000 iter use time {}ns", if_time);

//     sequence.set(1);
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < 100; i++)
//     {
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             sequence.set(1);
//             sequence.compare_and_set(1, 2);
//             sequence.compare_and_set(2, 1);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             sequence.set(1);
//             sequence.compare_and_set(1, 2);
//             sequence.compare_and_set(2, 1);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set(expected_value, new_value);
//         }
//         for (int i = 0; i < 10000; i++)
//         {
//             auto [expected_value, new_value] = list[i];
//             sequence.compare_and_set_if(new_value, expected_value);
//         }
//     }
//     auto end = std::chrono::high_resolution_clock::now();
//     auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//     fmt::println("10000000 iter use time {}ns", time);
//     return 0;
// }

// int main()
// {
// mozi::ring::mo_sequence_t sequence;
// sequence.set(1);
// fmt::println("{}", sequence.value());
// sequence.compare_and_set(1, 2);
// fmt::println("{}", sequence.value());
// sequence.compare_and_set(1, 2);
// fmt::println("{}", sequence.value());

// [[maybe_unused]] mozi::ring::mo_single_producer_sequencer_t sequencer{};
// sequencer.publish(1, 1);
// }
struct people
{
    std::string name;
};
void say_name([[maybe_unused]] uint8_t *bytes, void *data)
{
    auto p = static_cast<people *>(data);
    fmt::print("hello, my name is {}\n", p->name);
}
void single_use_poller()
{
    using mail = mozi::mail::mo_mail_t;
    using mail_factory = mozi::mail::mo_mail_factory_t;
    using mo_mail_translator = mozi::mail::mo_mail_translator_t;
    using producer = mozi::ring::mo_single_producer_sequencer_t<mail>;
    using ring_buffer = mozi::ring::mo_ring_buffer_t<mail, 8, producer, mail_factory, mo_mail_translator>;
    auto single_producer = ring_buffer::create_single_producer();
    auto poller = single_producer->create_poller();
    uint8_t *bytes = new uint8_t[1];
    people *h = new people{"rage"};
    mo_mail_translator pub_mail{bytes, h, say_name};
    single_producer->publish_event(pub_mail);
    spdlog::debug("publish_event");
    poller->poll(mozi::mail::mo_mail_read_t{});
}

void muilt_use_poller()
{
    spdlog::set_level(spdlog::level::debug);
    using mail = mozi::mail::mo_mail_t;
    using mail_factory = mozi::mail::mo_mail_factory_t;
    using mo_mail_translator = mozi::mail::mo_mail_translator_t;
    using producer = mozi::ring::mo_multi_producer_sequencer_t<mail, 4>;
    using ring_buffer = mozi::ring::mo_ring_buffer_t<mail, 4, producer, mail_factory, mo_mail_translator>;
    auto multi_producer = ring_buffer::create_multi_producer();
    auto poller = multi_producer->create_poller();

    // not overflow producer
    auto jthread_0 = std::jthread([&multi_producer]() {
        auto num = 0;
        while (100 > num)
        {
            uint8_t *bytes = new uint8_t[1];
            people *h = new people{"rage"};
            mo_mail_translator pub_mail{bytes, h, say_name};
            auto is_sucess = multi_producer->publish_event(pub_mail);
            spdlog::debug("publish_event:{}", is_sucess);
            fmt::println("");
            num += 1;
        }
    });

    auto jthread_1 = std::jthread([&multi_producer]() {
        auto num = 0;
        while (100 > num)
        {
            uint8_t *bytes = new uint8_t[1];
            people *h = new people{"rage"};
            mo_mail_translator pub_mail{bytes, h, say_name};
            auto is_sucess = multi_producer->publish_event(pub_mail);
            spdlog::debug("publish_event:{}", is_sucess);
            fmt::println("");
            num += 1;
        }
    });

    auto jthread_3 = std::jthread([&multi_producer]() {
        auto num = 0;
        while (100 > num)
        {
            uint8_t *bytes = new uint8_t[1];
            people *h = new people{"rage"};
            mo_mail_translator pub_mail{bytes, h, say_name};
            auto is_sucess = multi_producer->publish_event(pub_mail);
            spdlog::debug("publish_event:{}", is_sucess);
            fmt::println("");
            num += 1;
        }
    });
    // not overflow producer

    // uint8_t *bytes_7 = new uint8_t[1];
    // people *h_7 = new people{"rage"};
    // mo_mail_translator pub_mail_7{bytes_7, h_7, say_name};
    // is_sucess = multi_producer->publish_event(pub_mail_7);
    // spdlog::debug("publish_event:{}", is_sucess);
    // fmt::println("");

    spdlog::debug("buffer_size:{}", multi_producer->buffer_size());
    spdlog::debug("remaining_capacity:{}", multi_producer->remaining_capacity());

    // poller->poll(mozi::mail::mo_mail_read_t{});
    // poller->poll(mozi::mail::mo_mail_read_t{});
}

int main()
{
    muilt_use_poller();
    return 0;
}