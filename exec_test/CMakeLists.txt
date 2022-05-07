cmake_minimum_required(VERSION 3.10)
project(single_item_example)

set(CMAKE_CXX_STANDARD 14)

include_directories(../function_units/public)
link_directories(../function_units/public)
find_package(Threads REQUIRED)


file(
        GLOB_RECURSE single_item_example RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/signal_test.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/service_scheduler.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/fork_test.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/heartbeat.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/heart_beat_package.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/heart_check.cpp"

)

# 循环遍历每个含main的文件，分别编译生成执行文件
foreach(exec IN LISTS single_item_example)
    # Get file name without directory
    get_filename_component(exec ${exec} NAME_WE)
    add_executable(${exec} ${exec})
    target_link_libraries(${exec} public_lib Threads::Threads)
endforeach()

add_subdirectory(tools)
add_subdirectory(socket_tests)