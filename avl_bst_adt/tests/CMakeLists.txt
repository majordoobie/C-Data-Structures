add_executable(
        bst_testing_gtest
        bst_testing.cpp
)

target_link_libraries(
        bst_testing_gtest
        bst
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(bst_testing_gtest)