include(BuildUtils)

add_library(stack SHARED stack.c)
set_project_properties(stack ${CMAKE_CURRENT_SOURCE_DIR}/../include)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()