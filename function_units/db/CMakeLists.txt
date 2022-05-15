# project_lib name
project(mysql_lib)

include_directories(../public)

set(HEADER_FILES    _mysql.h      )
set(SOURCE_FILES    _mysql.cpp    )

# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE_DIR - where to find mysql.h, etc.
#  MYSQL_LIBRARIES   - List of libraries when using MySQL.
#  MYSQL_FOUND       - True if MySQL found.

IF (MYSQL_INCLUDE_DIR)
    # Already in cache, be silent
    SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
        /usr/local/include/mysql
        /usr/include/mysql
        )

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
        NAMES ${MYSQL_NAMES}
        PATHS /usr/lib /usr/local/lib
        PATH_SUFFIXES mysql
        )

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND TRUE)
    SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
    SET(MYSQL_FOUND FALSE)
    SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
    IF (NOT MYSQL_FIND_QUIETLY)
        MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
    ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
    IF (MYSQL_FIND_REQUIRED)
        MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
        MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
    ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
        MYSQL_LIBRARY
        MYSQL_INCLUDE_DIR
)

message(${MYSQL_INCLUDE_DIR})
message(${MYSQL_LIBRARY})

include_directories(${MYSQL_INCLUDE_DIR})

# add lib
add_library(mysql_lib STATIC ${SOURCE_FILES} ${HEADER_FILES}  )
target_link_libraries(mysql_lib  ${MYSQL_LIBRARY})
