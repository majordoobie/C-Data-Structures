include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/test)

add_executable(test_bst_target test_driver.c bst_test_functions.c hashtable_test_functions.c tree_map_avl_test_functions.c heap_test.c)
set_target_properties(test_bst_target PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(test_bst_target PUBLIC bst_lib heaplib.a tree_map_avl_lib check m rt pthread subunit)