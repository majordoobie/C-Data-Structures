#${CMAKE_SOURCE_DIR}/avl_bst_adt/src/bst.c

add_executable(
        avl_tree_map_testing_gtest
        avl_map_test.cpp
)

target_link_libraries(
        avl_tree_map_testing_gtest
        PUBLIC
        avl_map
)

include(GTestConfig)
AddTest(avl_tree_map_testing_gtest)
