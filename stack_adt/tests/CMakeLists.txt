add_executable(
        stack_adt_gtest
        stack_adt_gtest.cpp
)

target_link_libraries(
        stack_adt_gtest
        stack
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(stack_adt_gtest)
