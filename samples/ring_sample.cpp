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
    [[maybe_unused]] auto poller = single_producer->create_poller();
    uint8_t *bytes = new uint8_t[1];
    people *h = new people{"hello"};
    mo_mail_translator pub_mail{bytes, h, say_name};
    single_producer->publish_event(pub_mail);
    spdlog::info("publish_event");
    poller->poll(mozi::mail::mo_mail_read_t{});
}

// int main()
// {
// single_use_poller();
// return 0;
// }
// template <typename T, typename U> auto add(T t, U u)
// {
//     return t + u;
// }

#include <coroutine>
#include <iostream>
#include <queue>
#include <vector>

template <typename T> struct Generator
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type
    {
        T value;
        std::suspend_always yield_value(T val)
        {
            value = val;
            return {};
        }
        std::suspend_always initial_suspend()
        {
            return {};
        }
        std::suspend_never final_suspend() noexcept
        {
            return {};
        }
        void return_void()
        {
        }
        void unhandled_exception()
        {
            std::terminate();
        }
        Generator get_return_object()
        {
            return Generator(handle_type::from_promise(*this));
        }
    };

    handle_type coro;

    Generator(handle_type h) : coro(h)
    {
    }
    ~Generator()
    {
        if (coro)
            coro.destroy();
    }

    Generator(const Generator &) = delete;
    Generator &operator=(const Generator &) = delete;

    Generator(Generator &&o) noexcept : coro(std::exchange(o.coro, {}))
    {
    }
    Generator &operator=(Generator &&o) noexcept
    {
        if (this != &o)
        {
            if (coro)
                coro.destroy();
            coro = std::exchange(o.coro, {});
        }
        return *this;
    }

    T next()
    {
        if (!coro.done())
        {
            coro.resume();
            return coro.promise().value;
        }
        throw std::runtime_error("Generator is done!");
    }
};

Generator<int> cycle_pop_elements(std::vector<int> &arr)
{
    while (true)
    {
        if (!arr.empty())
        {
            int value = arr.front();
            arr.erase(arr.begin());
            co_yield value;
        }
        else
        {
            // Simulate wait for the array to be refilled
            co_await std::suspend_always{};
        }
    }
}

int main()
{
    std::vector<int> arr = {1, 2, 3, 4, 5};
    auto gen = cycle_pop_elements(arr);

    for (int i = 0; i < 5; ++i)
    {
        std::cout << gen.next() << std::endl;
    }

    // Simulate refilling the array
    arr = {6, 7, 8, 9, 10};

    for (int i = 0; i < 5; ++i)
    {
        std::cout << gen.next() << std::endl;
    }

    return 0;
}
