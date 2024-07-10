#include "fmt/core.h"
#include "mozi/core/actor.hpp"
#include "mozi/core/alias.hpp"
#include "mozi/core/ring/ring_buffer.hpp"
#include "mozi/core/ring/single_producer_sequencer.hpp"
#include "mozi/core/uv_actor.hpp"
#include "spdlog/spdlog.h"
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
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
    using mail = mozi::mo_mail_t;
    using mail_factory = mozi::mo_mail_factory_t;
    using mo_mail_translator = mozi::mo_mail_translator_t;
    using producer = mozi::ring::mo_single_producer_sequencer_t<mail>;
    using ring_buffer = mozi::mo_ring_buffer_t<mail, 8, producer, mail_factory, mo_mail_translator>;
    auto single_producer = ring_buffer::create_single_producer();
    auto poller = single_producer->create_poller();
    uint8_t *bytes = new uint8_t[1];
    people *h = new people{"rage"};
    mo_mail_translator pub_mail{bytes, h, say_name};
    single_producer->publish_event(pub_mail);
    spdlog::debug("publish_event");
    poller->poll(mozi::mo_mail_read_t{});
}

void del_int(int *p)
{
    spdlog::info("delete int");
    delete p;
}

void atomic_use()
{
    struct dog
    {
        dog()
        {
        }
        ~dog()
        {
            spdlog::info("delete dog");
        }
        std::unique_ptr<int, decltype(&del_int)> p{new int{1}, del_int};
    };
    struct animel : dog
    {
        animel(std::string name) : name{name}
        {
        }
        ~animel()
        {
            spdlog::info("delete {}", name);
        }

        std::string name;
    };
    auto a1 = std::make_shared<animel>("rage");
    auto a2 = std::make_shared<animel>("cpp");

    auto *p1 = new std::vector<std::shared_ptr<animel>>{};
    p1->push_back(a1);

    auto *p2 = new std::vector<std::shared_ptr<animel>>{};
    p2->push_back(a2);

    auto *p3 = new std::vector<std::shared_ptr<animel>>{};
    p3->push_back(a2);

    std::atomic<std::vector<std::shared_ptr<animel>> *> atomic_ptr;
    atomic_ptr.store(p1);
    spdlog::info("name {}", atomic_ptr.load()->at(0)->name);

    auto orign = atomic_ptr.load();
    atomic_ptr.compare_exchange_weak(orign, p2);
    spdlog::info("name {}", atomic_ptr.load()->at(0)->name);
    delete orign;

    orign = atomic_ptr.load();
    atomic_ptr.compare_exchange_weak(orign, p3);
    delete orign;
    delete atomic_ptr.load();
}

void shared_vec()
{
    class Test
    {
      public:
        ~Test()
        {
            spdlog::info("Test object destroyed\n");
        }
    };
    std::vector<std::shared_ptr<Test>> vec;

    vec.reserve(2);
    vec.reserve(1);

    vec.push_back(std::make_shared<Test>());
    vec.push_back(std::make_shared<Test>());

    spdlog::info("Before clear, vector size: {}", vec.size());

    vec.clear(); // 清理向量，减少shared_ptr的引用计数

    spdlog::info("After clear, vector size: {}", vec.size());
}

void muilt_use_poller()
{
    spdlog::set_level(spdlog::level::debug);
    struct uv_data
    {
        mozi::mo_uv_actor_t uv;
    };
    struct people
    {
    };
    auto uvdata = new uv_data{mozi::mo_uv_actor_t::create()};
    uvdata->uv.start();
    {
        auto f = []([[maybe_unused]] uint8_t *bytes, [[maybe_unused]] void *data) { spdlog::info("hello"); };

        // while (true)
        // {
        uint8_t *bytes = new uint8_t[1];
        people *h = new people{};
        mozi::mo_mail_translator_t pub_mail{bytes, h, f};
        uvdata->uv.publish_event(pub_mail);
        uvdata->uv.resume();
        // }

        // uvdata->uv.resume();
        // uvdata->uv.stop();
        // uvdata->uv.resume();
    }

    auto normal_actor = mozi::mo_actor_t::create();
    normal_actor.start();
    spdlog::info("end");
    delete uvdata;
}

int main()
{
    muilt_use_poller();
    return 0;
}