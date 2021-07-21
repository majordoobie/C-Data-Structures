cmake_minimum_required(VERSION 3.17)
project(TREE_EXAMPLE C)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 11)

add_subdirectory(src)
add_subdirectory(test)
