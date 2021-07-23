include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(bst_lib bst.c)
add_executable(main_exe ../../test/main.c)
target_link_libraries(main_exe PUBLIC bst_lib)