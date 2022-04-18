# project_lib name
project(json_lib)

include_directories(../)

set(HEADER_FILES    archiver.h      st_station_data.h)
set(SOURCE_FILES    archiver.cpp    st_station_data.cpp)

# add lib
add_library(json_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})