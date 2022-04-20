cmake_minimum_required(VERSION 3.0)
project(single_item_example)

set(CMAKE_CXX_STANDARD 14)

include_directories(../function_units/public)

find_package(Threads REQUIRED)

#   signal_test.cpp
#   service_scheduler.cpp
#   fork_test.cpp
#   heartbeat.cpp

set(test_item heartbeat.cpp)

add_executable(single_item_example ${test_item}  )

target_link_libraries(single_item_example public_lib)
target_link_libraries(single_item_example Threads::Threads)
