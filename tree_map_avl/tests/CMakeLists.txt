#${CMAKE_SOURCE_DIR}/binary_search_tree/src/bst.c

add_executable(
        avl_tree_map_gtest
        avl_map_test.cpp
)

target_link_libraries(
        avl_tree_map_gtest
        avl_map
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(avl_tree_map_gtest)
