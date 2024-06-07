cmake_minimum_required(VERSION 3.29)

project(samples LANGUAGES CXX)

add_executable(hello mozi.cpp)

target_link_libraries(hello PRIVATE mozi)

install(TARGETS hello RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")