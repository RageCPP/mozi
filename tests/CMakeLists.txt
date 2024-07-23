cmake_minimum_required(VERSION 3.29)

project(tests LANGUAGES CXX)

enable_testing()

add_executable(mozi_test
  ring.cpp
  scheduler.cpp)

target_link_libraries(
  mozi_test
  mozi
  GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(mozi_test)