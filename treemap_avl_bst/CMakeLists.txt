cmake_minimum_required(VERSION 3.20)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
project(avl_tree_map C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
ENDIF()

