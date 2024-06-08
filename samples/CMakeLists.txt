cmake_minimum_required(VERSION 3.29)

project(samples LANGUAGES CXX)

add_executable(hello mozi.cpp)
add_executable(chrono chrono.cpp)
add_executable(nid nid.cpp)

target_link_libraries(hello PRIVATE mozi)
target_link_libraries(chrono PRIVATE mozi)
target_link_libraries(nid PRIVATE mozi)

install(TARGETS hello RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS chrono RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS nid RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")