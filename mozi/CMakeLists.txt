cmake_minimum_required(VERSION 3.29)

project(mozi LANGUAGES CXX C)

include(CheckCXXSourceCompiles)

unset(HAS_COUNTER CACHE)
check_cxx_source_compiles("
  #ifdef __COUNTER__
      int main() { return 0; }
  #endif
" HAS_COUNTER)
if(HAS_COUNTER)
    message(STATUS "Compiler supports __COUNTER__")
else()
    message(WARNING "Compiler does not support __COUNTER__")
endif()

include(CheckPIESupported)
check_pie_supported()

set(MOZI_VARIABLES
  variables/mask.hpp
  variables/const_dec.hpp
  variables/string_view.hpp)

set(MOZI_COMPILE_HEADERS
  compile/portability_c.hpp
  compile/portability.hpp
  compile/attributes_cpp.hpp
  compile/preprocessor.hpp)

set(MOZI_UTILS_HEADERS
  exception.hpp
  expected.hpp
  traits.hpp
  unit.hpp
  utility.hpp
  mo_time.hpp)

set(MOZI_NID_HEADERS
  nid/worker_config.hpp
  nid/worker.hpp)

set(MOZI_HTTP_HEADERS
  http/llhttp/llhttp.h
  http/ada.h
  http/http.hpp
  http/responder.hpp
  http/router.hpp)
set(MOZI_HTTP_SOURCES
  http/llhttp/api.c
  http/llhttp/http.c
  http/llhttp/llhttp.c)

set(MOZI_CORE_DISRUPTOR_HEADERS
  core/disruptor/cursored.hpp
  core/disruptor/ring_buffer.hpp
  core/disruptor/sequence.hpp
  core/disruptor/gating_sequence.hpp
  core/disruptor/sequenced.hpp
  core/disruptor/sequencer.hpp
  core/disruptor/data_provider.hpp
  core/disruptor/event_poller.hpp
  core/disruptor/single_producer_sequencer.hpp
  core/disruptor/processing_sequence_barrier.hpp
  core/disruptor/utils.hpp)

set(MOZI_HEADERS
  core/common.hpp
  core/mo.hpp)

# add_library(${PROJECT_NAME} STATIC
#   ${MOZI_COMPILE_HEADERS}
#   ${MOZI_NID_HEADERS}
#   ${MOZI_VARIABLES}
#   ${MOZI_HEADERS}
#   ${MOZI_HTTP_HEADERS}
#   ${MOZI_HTTP_SOURCES}
#   ${MOZI_UTILS_HEADERS}
#   ${MOZI_CORE_DISRUPTOR_HEADERS})

add_library(${PROJECT_NAME} STATIC
  ${MOZI_COMPILE_HEADERS}
  ${MOZI_UTILS_HEADERS})

set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE FALSE)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  # target_compile_options(${PROJECT_NAME} PRIVATE )
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(${PROJECT_NAME} PUBLIC
  concurrentqueue
  libcuckoo::libcuckoo
  spdlog::spdlog_header_only
  date::date date::date-tz
  ssl
  pcre2-8 r3
  pq pqxx
  fmt::fmt
  libuv::uv_a
  jwt-cpp::jwt-cpp
  flatbuffers)

install(TARGETS ${PROJECT_NAME} spdlog_header_only
  EXPORT ${PROJECT_NAME}-config
  ARCHIVE DESTINATION "${MOZI_INSTALL_LIB_DIR}/mozi"
  LIBRARY DESTINATION "${MOZI_INSTALL_LIB_DIR}/mozi")

install(EXPORT ${PROJECT_NAME}-config
  NAMESPACE mozi::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mozi)


message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

get_target_property(MY_COMPILE_OPTIONS ${PROJECT_NAME} COMPILE_OPTIONS)
message(STATUS "Compile options for ${PROJECT_NAME}: ${MY_COMPILE_OPTIONS} ${CMAKE_CXX_FLAGS}")

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/pkg/date/include>
  $<BUILD_INTERFACE:${PCRE2_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${R3_INCLUDE_DIRS}>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/>)

install(FILES
  ${MOZI_COMPILE_HEADERS}
  DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/compile")
# install(FILES
#   ${MOZI_HEADERS}
#   DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi")
install(FILES
  ${MOZI_UTILS_HEADERS}
  DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/utils")
# install(FILES
#   ${MOZI_NID_HEADERS}
#   DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/nid")
# install(FILES
#   ${MOZI_HTTP_HEADERS}
#   DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/http")
# install(FILES
#   ${MOZI_VARIABLES}
#   DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/variables")
# install(FILES
#   ${MOZI_CORE_DISRUPTOR_HEADERS}
#   DESTINATION "${MOZI_INSTALL_INCLUDE_DIR}/mozi/core/disruptor")