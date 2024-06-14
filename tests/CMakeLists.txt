cmake_minimum_required(VERSION 3.29)

project(tests LANGUAGES CXX)

enable_testing()

add_executable(
  ring_test
  ring/sequence_about.cpp)

target_link_libraries(
  ring_test
  GTest::gtest_main
  mozi
)

include(GoogleTest)
gtest_discover_tests(ring_test)