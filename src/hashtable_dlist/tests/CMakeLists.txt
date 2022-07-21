add_executable(
        hashtable_testing_gtest
        hashtable_testing_gtest.cpp
)

target_link_libraries(
        hashtable_testing_gtest
        heap
        gtest
        gtest_main
)
include(BuildUtils)
GTest_add_target(hashtable_testing_gtest)
