add_library(avl_map SHARED tree_map_avl.c)

# Include the location of the header file. The header of bst is not needed
# because that target was built with PUBLIC qualifier
target_include_directories(
        avl_map
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Target relies on the bst library since it uses bst as its backbone
target_link_libraries(
        avl_map
        bst
)
