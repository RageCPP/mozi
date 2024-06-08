#pragma once

#include <cstdint>
namespace mozi::mask
{
namespace len
{
constexpr uint8_t mo_4bit = 4;
constexpr uint8_t mo_5bit = 5;
constexpr uint8_t mo_10bit = 10;
constexpr uint8_t mo_12bit = 12;
constexpr uint8_t mo_17bit = 17;
constexpr uint8_t mo_18bit = 18;
} // namespace len
namespace all
{
constexpr uint16_t mo_4bit = 0xF;
constexpr uint16_t mo_5bit = 0x1F;
constexpr uint16_t mo_10bit = 0x3FF;
constexpr uint16_t mo_12bit = 0xFFF;
constexpr uint32_t mo_17bit = 0x1FFFF;
constexpr uint32_t mo_18bit = 0x3FFFF;
} // namespace all
} // namespace mozi::mask