#pragma once
#include <chrono>
#include <cstdint>

namespace mozi::chrono
{

// System time zone but without time zone
namespace raw
{
inline auto now() noexcept
{
    return std::chrono::system_clock::now();
};
inline auto unsafe_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0,
                        uint8_t second = 0, uint16_t millisecond = 0) noexcept
{
    struct std::tm tm
    {
    };
    // [years since 1900](https://en.cppreference.com/w/cpp/chrono/c/tm)
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    return std::chrono::system_clock::from_time_t(std::mktime(&tm)) + std::chrono::milliseconds(millisecond);
};
inline int64_t timestamp(std::chrono::time_point<std::chrono::system_clock> timepoint) noexcept
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(timepoint.time_since_epoch()).count();
};
} // namespace raw

namespace local
{
inline auto unsafe_localtime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour = 0, uint8_t minute = 0,
                             uint8_t second = 0, uint16_t millisecond = 0) noexcept
{
    auto zone = std::chrono::current_zone();
    struct std::tm tm
    {
    };
    // [years since 1900](https://en.cppreference.com/w/cpp/chrono/c/tm)
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    return std::chrono::zoned_time(zone, std::chrono::system_clock::from_time_t(std::mktime(&tm)) +
                                             std::chrono::milliseconds(millisecond));
};
inline auto now() noexcept
{
    auto zone = std::chrono::current_zone();
    return std::chrono::zoned_time(zone, std::chrono::system_clock::now());
};
}; // namespace local

namespace utcz
{
template <typename Duration> inline int64_t timestamp(std::chrono::zoned_time<Duration> zonetime) noexcept
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(zonetime.get_local_time().time_since_epoch()).count();
};
} // namespace utcz
} // namespace mozi::chrono
