include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(../intersect main.c)
set_target_properties(../intersect PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(../intersect PUBLIC bst_lib tree_map_avl_lib intersect_lib)