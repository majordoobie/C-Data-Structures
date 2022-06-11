cmake_minimum_required(VERSION 3.20)
project(data_structures_c C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add the cmake modules directory to call functions in them
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(data_structures/avl_bst_adt)
add_subdirectory(data_structures/treemap_avl_bst)
add_subdirectory(data_structures/stack_adt)
add_subdirectory(data_structures/heap_adt)
add_subdirectory(data_structures/dlist_adt)
add_subdirectory(data_structures/queue_dlist)
add_subdirectory(data_structures/circular_list_dlist)
