#pragma once
#include "mozi/core/alias.hpp"
#include "mozi/net/http.hpp"
#include "mozi/nid/worker.hpp"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <memory>
#include <thread>
#include <uv.h>
namespace mozi
{
// id must <= 31
void run(int32_t port)
{
    uint8_t num_cores = std::thread::hardware_concurrency() / 2;
    spdlog::info("worker num: {}\n", num_cores);
    auto d_2020_01_01 = mozi::chrono::raw::unsafe_time(2020, 1, 1);
    auto d_2020_01_01_timestamp = mozi::chrono::raw::timestamp(d_2020_01_01);
    std::vector<std::jthread> threads;
    while (num_cores >= 1)
    {
        threads.emplace_back([num_cores, d_2020_01_01_timestamp, port]() {
            auto id = num_cores - 1;
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(num_cores, &cpuset);
            pthread_t current_thread = pthread_self();
            spdlog::info("cores {} on current_thread {} -- workid {}", num_cores, current_thread, id);
            if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0)
            {
                spdlog::error("Set thread affinity failed");
            }
            mo_route_t *tree = r3_tree_create(100);
            auto responders = std::make_unique<mozi::mo_responder_map_t>();
            auto nid_worker = std::make_unique<nid::mo_nid_worker_t>(d_2020_01_01_timestamp, id);
            auto loop_data = std::make_unique<mo_loop_data_t>(id, tree, responders.release(), nid_worker.release());
            auto loop = std::make_unique<uv_loop_t>();
            auto loop_raw = loop.release();
            uv_loop_init(loop_raw);
            loop_raw->data = loop_data.get();
            mozi::http::mo_http_listen(loop_raw, port + id);
        });
        num_cores -= 1;
    }
}
} // namespace mozi