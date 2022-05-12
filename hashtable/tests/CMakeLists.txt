add_executable(
        hashtable_testing_gtest
        hashtable_testing_gtest.cpp
)

target_link_libraries(
        hashtable_testing_gtest
        hashtable
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(hashtable_testing_gtest)
