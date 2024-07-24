cmake_minimum_required(VERSION 3.20)

project(samples LANGUAGES CXX)

add_executable(use_redis use_redis.cpp)
target_link_libraries(use_redis PRIVATE pcre2-8 r3 mozi)
install(TARGETS use_redis RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")

# target_link_libraries(http_sample PRIVATE pcre2-8 r3 mozi)

# install(TARGETS http_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
