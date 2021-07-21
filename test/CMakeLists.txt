include_directories(${TREE_EXAMPLE_SOURCE_DIR}/include)
#set(CHECK_INSTALL_DIR "/usr/local/include/check.h")
#link_directories(/usr/include)

add_executable(test_bst_target test.c ../src/bst.c)
target_link_libraries(test_bst_target check m rt pthread subunit)
#find_library(CHECK_LIB check)
#target_link_libraries(test_bst_target PUBLIC ${CHECK_LIB} m rt pthread subunit)