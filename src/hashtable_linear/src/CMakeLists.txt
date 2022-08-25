include(BuildUtils)

add_library(hashtable SHARED hashtable.c)
set_project_properties(hashtable ${CMAKE_CURRENT_SOURCE_DIR}/../include)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()