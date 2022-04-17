# project_lib name
project(public_lib)

set(HEADER_FILES    _cmpublic.h _public.h)
set(SOURCE_FILES    _public.cpp)

# add lib
add_library(public_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})