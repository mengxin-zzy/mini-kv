cmake_minimum_required(VERSION 3.16)
project(mini-kv CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_library(kvcore src/db.cpp src/skiplist.cpp src/pool.cpp)
target_include_directories(kvcore PUBLIC include)

add_executable(mini_kv main.cpp)
target_link_libraries(mini_kv PRIVATE kvcore)
enable_testing()
find_package(GTest REQUIRED)

add_executable(test_db tests/test_db.cpp)
target_link_libraries(test_db PRIVATE kvcore GTest::gtest_main)
add_test(NAME kv COMMAND test_db)
target_link_libraries(kvcore PUBLIC z)

add_executable(test_skiplist tests/test_skiplist.cpp)
target_link_libraries(test_skiplist PRIVATE kvcore GTest::gtest_main)
target_include_directories(test_skiplist PRIVATE include)
add_test(NAME skiplist COMMAND test_skiplist)

add_executable(bench_containers bench/bench_containers.cpp)
target_link_libraries(bench_containers PRIVATE kvcore)

add_executable(echo_server bench/echo_server.cpp)