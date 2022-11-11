cmake_minimum_required(VERSION 3.20)
project(dsa_c VERSION 1.0 LANGUAGES C CXX)

# Add the cmake modules directory to call functions in them
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Set boiler settings
include(cmake/BuildUtils.cmake)
SetBoilerSettings()

option(BUILD_DSA_C_GTESTS "Build GTests for DSA_C" ON)
if (BUILD_DSA_C_GTESTS)
    # If debug is enabled make sure to include CTest at the root. This will allow
    # the ctest config to be placed at the root of the build directory
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        include(CTest)
    ENDIF()
ENDIF()

add_subdirectory(deps)
add_subdirectory(src)
add_subdirectory(test)
#add_subdirectory(src/avl_bst_adt/src)
#add_subdirectory(src/treemap_avl_bst/src)
#add_subdirectory(src/stack_adt/src)
#add_subdirectory(src/heap_adt/src)
#add_subdirectory(src/queue_dlist/src)
#add_subdirectory(src/circular_list_dlist/src)
#add_subdirectory(src/graph_dlist/src)
#add_subdirectory(src/hashtable_linear/src)