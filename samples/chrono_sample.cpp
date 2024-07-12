// #include <chrono>
// #include <fmt/core.h>
// #include <iostream>
// #include <mozi/utils/chrono.hpp>
// int main()
// {
//     // auto d_2000_01_01 = mozi::chrono::local::unsafe_localtime(2000, 1, 1);
//     // fmt::println("2000 01 01 timestamp millis:{}", mozi::chrono::utc::timestamp_millis(d_2000_01_01));
//     // auto d_now = mozi::chrono::utc::now::timestamp_millis();
//     // fmt::println("now timestamp millis:{}", d_now);

//     auto hour_seconds = 60 * 60;
//     auto zone = std::chrono::current_zone();
//     std::chrono::zoned_time zonetime(zone, std::chrono::system_clock::now());
//     fmt::println("zone get_time_zone name: {}", zonetime.get_time_zone()->name());
//     fmt::println("zone abbrev: {}", zonetime.get_info().abbrev);
//     std::cout << "zone Time: " << zonetime << std::endl;
//     fmt::println("====================================================================");
//     struct std::tm timeinfo = {};
//     timeinfo.tm_year = 2000 - 1900;
//     timeinfo.tm_mon = 1 - 1;
//     timeinfo.tm_mday = 1;
//     timeinfo.tm_hour = 0;
//     timeinfo.tm_min = 0;
//     timeinfo.tm_sec = 0;
//     time_t time = std::mktime(&timeinfo);
//     auto time_point = std::chrono::system_clock::from_time_t(time) + std::chrono::milliseconds(0);
//     std::chrono::zoned_time zonetime2(zone, time_point);
//     fmt::println("zone2 get_time_zone name: {}", zonetime2.get_time_zone()->name());
//     fmt::println("zone2 abbrev: {}", zonetime2.get_info().abbrev);
//     std::cout << "zone2 Time: " << zonetime2 << std::endl;
//     fmt::println("====================================================================");
//     auto _1 = std::chrono::duration_cast<std::chrono::seconds>(time_point.time_since_epoch()).count();
//     auto _2 =
//     std::chrono::duration_cast<std::chrono::seconds>(zonetime2.get_local_time().time_since_epoch()).count();
//     fmt::println("zone2 time_point timestamp: {}", _1);
//     fmt::println("zone2 zonetime2 timestamp: {}", _2);
//     fmt::println("offeset: {}", zone->get_info(std::chrono::system_clock::now()).offset.count() / hour_seconds);
//     fmt::println("delta :{}", (_2 - _1) / hour_seconds);

//     // fmt::println("Current time zone:{}", zone->name());
//     // auto d_2000_01_01_08 = mozi::chrono::local::unsafe_localtime(2000, 1, 1, 8);
//     // fmt::println("2000 01 01 08 timestamp millis:{}", mozi::chrono::utc::timestamp_ms(d_2000_01_01_08));
// }
