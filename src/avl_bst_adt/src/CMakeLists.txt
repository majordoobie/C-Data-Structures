include(BuildUtils)

add_library(bst STATIC bst.c)
set_project_properties(bst ${CMAKE_CURRENT_SOURCE_DIR}/../include)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()