include(BuildUtils)

add_library(dl_list SHARED dl_list.c dl_iter.c)
set_project_properties(dl_list ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(dl_list utils)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()