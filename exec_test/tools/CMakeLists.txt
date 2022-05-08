cmake_minimum_required(VERSION 3.10)
project(tools_example)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../function_units/public)
include_directories(../../function_units/ftp)


find_package(Threads REQUIRED)


file(
        GLOB_RECURSE single_item_example RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/gzip_files.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/delete_files.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ftp_get_files.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/ftp_put_files.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_file_client.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/tcp_file_server.cpp"
)

# 循环遍历每个含main的文件，分别编译生成执行文件
foreach(exec IN LISTS single_item_example)
    # Get file name without directory
    get_filename_component(exec ${exec} NAME_WE)
    add_executable(${exec} ${exec})
    target_link_libraries(${exec} public_lib Threads::Threads ftp_lib )
endforeach()
