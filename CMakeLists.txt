cmake_minimum_required(VERSION 3.20)
project(data_structures_c C CXX)

# Uncomment this line if you need some serious debugging of the make process
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add the cmake modules directory to call functions in them
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# Create the variable for where all the tests will go in the build tree
# this will make it easier to execute all the test binaries
set(TEST_BUILD_PATH ${CMAKE_BINARY_DIR}/test_bin)

# If debug is enabled make sure to include CTest at the root. This will allow
# the ctest config to be placed at the root of the build directory
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
ENDIF()

add_subdirectory(deps)
add_subdirectory(src/utils)
add_subdirectory(src/avl_bst_adt/src)
add_subdirectory(src/treemap_avl_bst/src)
add_subdirectory(src/dlist_adt/src)
add_subdirectory(src/stack_adt/src)
add_subdirectory(src/heap_adt/src)
add_subdirectory(src/queue_dlist/src)
add_subdirectory(src/circular_list_dlist/src)
add_subdirectory(src/graph_dlist/src)
add_subdirectory(src/hashtable_linear/src)