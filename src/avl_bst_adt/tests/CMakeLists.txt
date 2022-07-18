add_executable(
        bst_testing_gtest
        avl_bst_adt_gtest.cpp
)

target_link_libraries(
        bst_testing_gtest
        PUBLIC
        bst
)

include(GTestConfig)
AddTest(bst_testing_gtest)