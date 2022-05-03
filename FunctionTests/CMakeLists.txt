# project name
project(FunctionTests)

#add_subdirectory(/googletest-1.10.x)
# gtest source
set(gtest_SOURCE_DIR /googletest-1.10.x/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# google test sources
include_directories(public_tests)
include_directories(json_tests)
include_directories(ftp_tests)

include_directories(../function_units/public)


set(PUBLIC_TEST_HEADER public_tests/test.h  )
set(PUBLIC_TEST_SOURCE public_tests/test.cpp  )

set(JSON_TEST_HEADER   )
set(JSON_TEST_SOURCE json_tests/test.cpp  )

set(FTP_TEST_HEADER   )
set(FTP_TEST_SOURCE ftp_tests/test.cpp  )



set(gtest_SOURCE_DIR googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FunctionTests ${PUBLIC_TEST_HEADER} ${PUBLIC_TEST_SOURCE}
                             ${JSON_TEST_HEADER}   ${JSON_TEST_SOURCE}
                             ${FTP_TEST_HEADER}    ${FTP_TEST_SOURCE}

        )

target_link_libraries(FunctionTests public_lib)
target_link_libraries(FunctionTests json_lib)
target_link_libraries(FunctionTests ftp_lib)

target_link_libraries(FunctionTests gtest gtest_main)
