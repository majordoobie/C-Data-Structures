add_executable(
        bst_testing_gtest
        bst_testing.cpp
)

target_link_libraries(
        bst_testing_gtest
        PUBLIC
        bst
)

include(GTestConfig)
AddTest(bst_testing_gtest)