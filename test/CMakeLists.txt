include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/test)

add_executable(test_bst_target bst_test_functions.c bst_test.c)

target_link_libraries(test_bst_target PUBLIC bst_lib check m rt pthread subunit)