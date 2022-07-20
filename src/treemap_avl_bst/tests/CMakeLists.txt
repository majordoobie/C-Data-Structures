#${CMAKE_SOURCE_DIR}/avl_bst_adt/src/bst.c

add_executable(
        avl_tree_map_testing_gtest
        treemap_avl_bst_gtest.cpp
)

target_link_libraries(
        avl_tree_map_testing_gtest
        PUBLIC
        avl_map
)

include(BuildUtils)
GTest_add_target(avl_tree_map_testing_gtest)
