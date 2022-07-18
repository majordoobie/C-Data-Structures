cmake_minimum_required(VERSION 3.20)

project(doubly_linked_list_adt C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src)
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
ENDIF()

set(dlist_include_dirs
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )
include_directories(${dlist_include_dirs})