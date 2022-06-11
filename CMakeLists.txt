cmake_minimum_required(VERSION 3.20)
project(data_structures_c C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add the cmake modules directory to call functions in them
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_subdirectory(avl_bst_adt)
add_subdirectory(treemap_avl_bst)
add_subdirectory(stack_adt)
add_subdirectory(heap_adt)
add_subdirectory(dlist_adt)
add_subdirectory(queue_dlist)
add_subdirectory(circular_list_dlist)
