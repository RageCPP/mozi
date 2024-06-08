#include "fmt/core.h"
#include "mozi/nid/worker.hpp"
#include "mozi/utils/chrono.hpp"
#include <chrono>

int main()
{
    /** error base time test */
    // mozi::nid::mo_nid_worker_c worker(0, 0);
    // worker.next_id();

    /** error worker id test */
    // auto d_2000_01_01 = mozi::chrono::raw::unsafe_time(2000, 1, 1);
    // auto d_2000_01_01_timestamp = mozi::chrono::raw::timestamp(d_2000_01_01);
    // mozi::nid::mo_nid_worker_c worker(d_2000_01_01_timestamp, 16);

    /** gen id output */
    auto d_2000_01_01 = mozi::chrono::raw::unsafe_time(2000, 1, 1);
    auto d_2000_01_01_timestamp = mozi::chrono::raw::timestamp(d_2000_01_01);
    mozi::nid::mo_nid_worker_c worker_1(d_2000_01_01_timestamp, 0);
    mozi::nid::mo_nid_worker_c worker_2(d_2000_01_01_timestamp, 1);
    fmt::println("worker_1 id:{}", worker_1.next_id());
    fmt::println("worker_2 id:{}", worker_2.next_id());
    for (int i = 0; i < 10; i++)
    {
        auto id = worker_1.next_id();
        fmt::println("id:{}", id);
    }
    // id:3234389257359458304
    // id:3234389257359458305
    // id:3234389257359458306
    // id:3234389257359458307
    // id:3234389257359458308
    // id:3234389257359458309 0 01011001110001011011011111001001001111001 0000 000000000000000101 worker_id:0
    // id:3234389257359458310 0 01011001110001011011011111001001001111001 0000 000000000000000110
    // id:3234389257359458311 0 01011001110001011011011111001001001111001 0000 000000000000000111
    // id:3234389257359458312 0 01011001110001011011011111001001001111001 0000 000000000000001000
    // id:3234389257359458313 0 01011001110001011011011111001001001111001 0000 000000000000001001
    // 0 01011001110001011011111010010010100101010 0001 000000000000000000 worker_id:1 4位worker_id

    // 0 01011001110001011100111011001001111010111 00001 00000000000000000 worker_id:1 5位worker_id

    auto empty_start = std::chrono::high_resolution_clock::now();
    auto e = 10 + 10;
    auto empty_end = std::chrono::high_resolution_clock::now();
    auto empty = std::chrono::duration_cast<std::chrono::nanoseconds>(empty_end - empty_start).count();
    fmt::println("empty use time {}ns", empty);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i <= 131071; i++)
    {
        [[maybe_unused]] auto _ = worker_2.next_id();
    }
    auto end = std::chrono::high_resolution_clock::now();
    fmt::println("131072 iter use time: {}ns",
                 std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() - empty);
    fmt::println("{}", e);
    return 0;
}