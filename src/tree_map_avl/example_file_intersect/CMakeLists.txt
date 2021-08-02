cmake_minimum_required(VERSION 3.17)
project(bst_adt C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)

add_subdirectory(src/bst)
add_subdirectory(src/tree_map_avl)
add_subdirectory(src/intersect)
add_subdirectory(src/intersect_cli)
add_subdirectory(src/)
add_subdirectory(test)
