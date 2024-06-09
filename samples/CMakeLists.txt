cmake_minimum_required(VERSION 3.29)

project(samples LANGUAGES CXX)

add_executable(hello mozi.cpp)
add_executable(chrono chrono.cpp)
add_executable(nid nid.cpp)
add_executable(http http.cpp)
add_executable(disruptor disruptor.cpp)

target_link_libraries(hello PRIVATE mozi)
target_link_libraries(chrono PRIVATE mozi)
target_link_libraries(nid PRIVATE mozi)
target_link_libraries(http PRIVATE pcre2-8 r3 mozi)
target_link_libraries(disruptor PRIVATE mozi)

install(TARGETS hello RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS chrono RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS nid RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS http RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS disruptor RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")