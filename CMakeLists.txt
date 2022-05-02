cmake_minimum_required(VERSION 3.20)
project(data_structures_c C)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# If in debug mode, then fetch the google test framework for testing
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
    enable_testing()
ENDIF()

add_subdirectory(binary_search_tree)