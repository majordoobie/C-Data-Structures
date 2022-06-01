add_executable(
        queue_testing_gtest
        queue_testing_gtest.cpp
)

target_link_libraries(
        queue_testing_gtest
        queue
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(queue_testing_gtest)
