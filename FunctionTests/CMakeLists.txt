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
include_directories(socket_tests)
include_directories(db_tests/mysql_tests)


include_directories(../function_units/public)

set(PUBLIC_TEST_HEADER public_tests/test.h  )
set(PUBLIC_TEST_SOURCE public_tests/test.cpp  )

set(JSON_TEST_HEADER   )
set(JSON_TEST_SOURCE json_tests/test.cpp  )

set(FTP_TEST_HEADER   )
set(FTP_TEST_SOURCE ftp_tests/test.cpp  )

set(SOCKET_TEST_HEADER socket_tests/tests.h  )
set(SOCKET_TEST_SOURCE socket_tests/tests.cpp  )

set(MYSQL_TEST_HEADER    )
set(MYSQL_TEST_SOURCE db_tests/mysql_tests/test.cpp  )

set(gtest_SOURCE_DIR googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(FunctionTests ${PUBLIC_TEST_HEADER} ${PUBLIC_TEST_SOURCE}
                             ${JSON_TEST_HEADER}   ${JSON_TEST_SOURCE}
                             ${FTP_TEST_HEADER}    ${FTP_TEST_SOURCE}
                             ${SOCKET_TEST_HEADER} ${SOCKET_TEST_SOURCE}
                             ${MYSQL_TEST_HEADER}  ${MYSQL_TEST_SOURCE}

        )

target_link_libraries(FunctionTests public_lib)
target_link_libraries(FunctionTests json_lib)
target_link_libraries(FunctionTests ftp_lib)
target_link_libraries(FunctionTests mysql_lib)

target_link_libraries(FunctionTests gtest gtest_main)
