add_executable(
        dlinked_list_testing_gtest
        dlinked_list_testing_gtest.cpp
)

target_link_libraries(
        dlinked_list
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(dlinked_list_testing_gtest)
