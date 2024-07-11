#pragma once
/*
 * 系统具有实际上限 cat /proc/sys/net/core/somaxconn
 * 修改 echo 1024 > /proc/sys/net/core/somaxconn
 */

#include <atomic>
#include <cstdint>

constexpr int MAX_REQUEST_SIZE = 1024; // unit char
constexpr int DEFAULT_BACKLOG = 10000;
constexpr uint16_t BITS_8 = 256;
constexpr uint16_t BITS_13 = 8192;
constexpr std::memory_order relaxed = std::memory_order_relaxed;
constexpr std::memory_order acquire = std::memory_order_acquire;
constexpr std::memory_order release = std::memory_order_release;
constexpr std::memory_order seq_cst = std::memory_order_seq_cst;
constexpr std::memory_order consume = std::memory_order_consume;
