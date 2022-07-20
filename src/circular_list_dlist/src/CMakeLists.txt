include(BuildUtils)

add_library(circular_list SHARED circular_list.c)
set_project_properties(circular_list ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(circular_list PUBLIC dl_list)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()