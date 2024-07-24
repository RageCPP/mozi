cmake_minimum_required(VERSION 3.20)

project(webserver LANGUAGES CXX)

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(libpqxx
  GIT_REPOSITORY git@github.com:jtv/libpqxx.git
  GIT_TAG 2ffb711bc4e9b086bf0c549a9c22b19f61ab5476 # v7.9.0
  OVERRIDE_FIND_PACKAGE
  GIT_SHALLOW ON
)
FetchContent_Declare(fmt
  GIT_REPOSITORY git@github.com:fmtlib/fmt.git
  GIT_TAG e69e5f977d458f2650bb346dadf2ad30c5320281 # v10.2.1
  GIT_SHALLOW ON
)
FetchContent_Declare(jwt
  GIT_REPOSITORY git@github.com:Thalhammer/jwt-cpp.git
  GIT_TAG 08bcf77a687fb06e34138e9e9fa12a4ecbe12332 # v0.7.0
  GIT_SHALLOW ON
)
FetchContent_Declare(concurrentqueue
  GIT_REPOSITORY git@github.com:cameron314/concurrentqueue.git
  GIT_TAG 6dd38b8a1dbaa7863aa907045f32308a56a6ff5d # v1.0.4
  GIT_SHALLOW ON
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY git@github.com:gabime/spdlog.git
  GIT_TAG 27cb4c76708608465c413f6d0e6b8d99a4d84302
  GIT_SHALLOW ON
)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(nlohmann_json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_Declare(llhttp URL https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz)
FetchContent_MakeAvailable(
  libpqxx
  jwt
  nlohmann_json
  fmt
  concurrentqueue
  spdlog
  llhttp
  googletest
)

set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/libuv")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/hiredis")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/cuckoo")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/pcre2")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/r3")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${CMAKE_SOURCE_DIR}/pkg/date")

find_package(date REQUIRED)
find_package(libuv REQUIRED)
find_package(libcuckoo REQUIRED)
find_package(hiredis REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PCRE2 REQUIRED libpcre2-8)
pkg_check_modules(R3 REQUIRED r3)

# include_directories(${PCRE2_INCLUDE_DIRS})
link_directories(${PCRE2_LIBRARY_DIRS})
message(STATUS "PCRE2_INCLUDE_DIRS: ${PCRE2_INCLUDE_DIRS}")
message(STATUS "PCRE2_LIBRARY_DIRS: ${PCRE2_LIBRARY_DIRS}")

# include_directories(${R3_INCLUDE_DIRS})
link_directories(${R3_LIBRARY_DIRS})
message(STATUS "R3_INCLUDE_DIRS: ${R3_INCLUDE_DIRS}")
message(STATUS "R3_LIBRARY_DIRS: ${R3_LIBRARY_DIRS}")

# 下面这两行会在编译命令中增加 -std=gnu++23 但是 clang-tidy 不支持此参数 使用 CMAKE_CXX_FLAGS 中的 -std=gnu++2b 就行了
# set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: 学习加 -g 后如何调试
# -std=gnu++23
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++2b -O2 -Wall -Wextra -Wpedantic -Werror")
option(CMAKE_VERBOSE_MAKEFILE "show compilation process" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "compile_commands.json generate")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(MOZI_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
set(MOZI_INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR})
set(MOZI_INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR})

add_subdirectory(mozi)
add_subdirectory(samples)
add_subdirectory(tests)