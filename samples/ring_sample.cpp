#include "fmt/core.h"
#include "mozi/core/mail_cache.hpp"
#include "mozi/core/ring/yield_wait_strategy.hpp"
#include <chrono>
#include <cstdint>
#include <random>
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

int main()
{
    [[maybe_unused]] auto info = mozi::mail::mo_mail_factory_t::create_instance();
    [[maybe_unused]] auto yield_wait_strategy = mozi::ring::yield_wait_strategy_t<uint8_t>{};
}
template <typename T, typename U> auto add(T t, U u)
{
    return t + u;
}