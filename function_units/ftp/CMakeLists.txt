# project_lib name
project(ftp_lib)

include_directories(../public)

set(HEADER_FILES    ftplib.h      _ftp.h)
set(SOURCE_FILES    ftplib.c    _ftp.cpp)


# add lib
add_library(ftp_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})
