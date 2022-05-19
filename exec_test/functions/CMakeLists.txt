cmake_minimum_required(VERSION 3.10)
project(function_tests)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../function_units)


find_package(Threads REQUIRED)


file(
        GLOB_RECURSE single_item_example RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/simulation_gen_data.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/insert_stationcode_to_db.cpp"

)

# 循环遍历每个含main的文件，分别编译生成执行文件
foreach(exec IN LISTS single_item_example)
    # Get file name without directory
    get_filename_component(exec ${exec} NAME_WE)
    add_executable(${exec} ${exec}.cpp)
    target_link_libraries(${exec}  Threads::Threads )
    target_link_libraries(${exec}  public_lib)
    target_link_libraries(${exec}  json_lib)
    target_link_libraries(${exec}  ftp_lib)
    target_link_libraries(${exec}  mysql_lib)
endforeach()
