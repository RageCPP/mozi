cmake_minimum_required(VERSION 3.29)

project(tests LANGUAGES CXX)

enable_testing()

add_executable(
  disruptor_test
  disruptor/sequence_about.cpp)

target_link_libraries(
  disruptor_test
  GTest::gtest_main
  mozi
)

include(GoogleTest)
gtest_discover_tests(disruptor_test)