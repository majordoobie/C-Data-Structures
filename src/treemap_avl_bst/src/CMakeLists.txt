include(BuildUtils)

add_library(avl_map STATIC tree_map_avl.c)
set_project_properties(avl_map ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(avl_map PUBLIC bst)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()