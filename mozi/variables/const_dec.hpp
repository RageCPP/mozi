#pragma once
/*
 * 系统具有实际上限 cat /proc/sys/net/core/somaxconn
 * 修改 echo 1024 > /proc/sys/net/core/somaxconn
 */

#include <cstdint>

constexpr int MAX_REQUEST_SIZE = 1024; // unit char
constexpr int DEFAULT_BACKLOG = 10000;
constexpr uint16_t BITS_13 = 8192;