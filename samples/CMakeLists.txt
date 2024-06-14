cmake_minimum_required(VERSION 3.29)

project(samples LANGUAGES CXX)

add_executable(hello_sample mozi_sample.cpp)
add_executable(chrono_sample chrono_sample.cpp)
add_executable(nid_sample nid_sample.cpp)
add_executable(http_sample http_sample.cpp)
add_executable(ring_sample ring_sample.cpp)
add_executable(crtp_sample CRTP.cpp)

target_link_libraries(hello_sample PRIVATE mozi)
target_link_libraries(chrono_sample PRIVATE mozi)
target_link_libraries(nid_sample PRIVATE mozi)
target_link_libraries(http_sample PRIVATE pcre2-8 r3 mozi)
target_link_libraries(ring_sample PRIVATE mozi)

install(TARGETS hello_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS chrono_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS nid_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS http_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS ring_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")
install(TARGETS crtp_sample RUNTIME DESTINATION "${MOZI_INSTALL_BIN_DIR}")