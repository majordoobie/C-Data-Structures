add_executable(
        circular_list_gtest
        circular_list_gtest.cpp
)

target_link_libraries(
        circular_list_gtest
        circular_list
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(circular_list_gtest)
