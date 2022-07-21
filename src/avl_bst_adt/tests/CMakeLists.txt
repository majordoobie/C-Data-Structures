add_executable(
        bst_testing_gtest
        avl_bst_adt_gtest.cpp
)

target_link_libraries(
        bst_testing_gtest
        PUBLIC
        bst
)

include(BuildUtils)
GTest_add_target(bst_testing_gtest)