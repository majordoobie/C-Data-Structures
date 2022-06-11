add_executable(
        stack_testing_gtest
        stack_adt_gtest.cpp
)

target_link_libraries(
        stack_testing_gtest
        PUBLIC
        stack
)

include(GTestConfig)
AddTest(stack_testing_gtest)
