cmake_minimum_required(VERSION 3.10)
project(socket_tests)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../function_units/public)


find_package(Threads REQUIRED)


file(
        GLOB_RECURSE single_item_example RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_proc_server.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_proc_client.cpp"

)

# 循环遍历每个含main的文件，分别编译生成执行文件
foreach(exec IN LISTS single_item_example)
    # Get file name without directory
    get_filename_component(exec ${exec} NAME_WE)
    add_executable(${exec} ${exec})
    target_link_libraries(${exec} public_lib Threads::Threads )
endforeach()
