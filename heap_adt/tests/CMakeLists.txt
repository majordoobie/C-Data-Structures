add_executable(
        heap_testing_gtest
        heap_testing_gtest.cpp
)

target_link_libraries(
        heap_testing_gtest
        heap
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(heap_testing_gtest)
