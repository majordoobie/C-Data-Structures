include_directories(${TREE_EXAMPLE_SOURCE_DIR}/include ${TREE_EXAMPLE_SOURCE_DIR}/test)
add_executable(test_bst_target test.c test_functions.c)

target_link_libraries(test_bst_target PUBLIC ${CHECK_LIB} bst_lib m rt pthread subunit)

#target_include_directories(
#        test_bst_target
#        "${TREE_EXAMPLE_SOURCE_DIR}/include"
#)


#include_directories(${TREE_EXAMPLE_SOURCE_DIR}/include ${TREE_EXAMPLE_SOURCE_DIR}/src)
#set(CHECK_INSTALL_DIR "/usr/local/include/check.h")
#link_directories(/usr/include)

#add_executable(test_bst_target test.c bst.c test_functions.c test_functions.h)
#target_link_libraries(test_bst_target check m rt pthread subunit)
#find_library(CHECK_LIB check)
#target_link_libraries(test_bst_target PUBLIC ${CHECK_LIB} m rt pthread subunit)