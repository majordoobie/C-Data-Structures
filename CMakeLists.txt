cmake_minimum_required(VERSION 3.20)
project(data_structures_c C CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -pedantic -Werror -D_FORTIFY_SOURCE=2 -fpie -Wl,-pie -fpic -shared )

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# If in debug mode, then fetch the google test framework for testing
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment)
    add_link_options(-g3 -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
    )
    FetchContent_MakeAvailable(googletest)
    enable_testing()
ENDIF()

add_subdirectory(avl_binary_search_tree)
add_subdirectory(tree_map_avl)
add_subdirectory(stack_adt)
add_subdirectory(heap)
add_subdirectory(hashtable)
