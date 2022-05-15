cmake_minimum_required(VERSION 3.0)
project(study_idc)

set(CMAKE_CXX_STANDARD 14)

# 编译google test生成libtest.a静态库
add_subdirectory(googletest-1.10.x)
#add_subdirectory(ini)
#add_subdirectory(sql)
add_subdirectory(function_units/public)
add_subdirectory(function_units/json)
add_subdirectory(function_units/ftp)
add_subdirectory(function_units/db)


add_subdirectory(exec_test)


include_directories(ini)
include_directories(sql)
include_directories(function_units)
include_directories(function_units/rapidjson)

add_executable(study_idc main.cpp )
target_link_libraries(study_idc public_lib)
target_link_libraries(study_idc json_lib)
target_link_libraries(study_idc ftp_lib)
target_link_libraries(study_idc mysql_lib)

add_subdirectory(FunctionTests)