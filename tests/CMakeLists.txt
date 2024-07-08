cmake_minimum_required(VERSION 3.29)

project(tests LANGUAGES CXX)

enable_testing()

add_executable(ring_test ring.cpp)

target_link_libraries(
  ring_test
  mozi
  GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ring_test)