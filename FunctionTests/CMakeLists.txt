# project name
project(FunctionTests)

#add_subdirectory(/googletest-1.10.x)
# gtest source
set(gtest_SOURCE_DIR /googletest-1.10.x/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# google test sources
include_directories(public_tests)

# test

set(PUBLIC_TEST_HEADER public_tests/test.h  )
set(PUBLIC_TEST_SOURCE public_tests/test.cpp  )

set(gtest_SOURCE_DIR googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FunctionTests ${PUBLIC_TEST_HEADER} ${PUBLIC_TEST_SOURCE})

target_link_libraries(FunctionTests public_lib)
target_link_libraries(FunctionTests gtest gtest_main)
