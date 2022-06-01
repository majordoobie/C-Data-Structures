add_executable(
        linked_list_testing_gtest
        linked_list_testing_gtest.cpp
)

target_link_libraries(
        linked_list_testing_gtest
        l_list
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(linked_list_testing_gtest)
