include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/test)

add_executable(test_bst_target bst_test.c bst_test_functions.c)
set_target_properties(test_bst_target PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(test_bst_target PUBLIC bst_lib check m rt pthread subunit)