include(BuildUtils)

add_library(graph_dlist SHARED graph_dlist.c)
target_link_libraries(graph_dlist dl_list heap)
set_project_properties(graph_dlist ${CMAKE_CURRENT_SOURCE_DIR}/../include)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()