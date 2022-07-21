include(BuildUtils)

add_library(heap SHARED heap.c)
set_project_properties(heap ${CMAKE_CURRENT_SOURCE_DIR}/../include)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()