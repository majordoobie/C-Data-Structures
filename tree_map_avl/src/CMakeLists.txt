add_library(avl_map STATIC tree_map_avl.c)

# Set the directory to where the bst include dir is. This is needed
# to use the bst library
set(
        bst_include_dir
        ${CMAKE_SOURCE_DIR}/binary_search_tree/include
)

# Test to public so that gtest can have access
target_include_directories(
        avl_map
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${bst_include_dir}
)
