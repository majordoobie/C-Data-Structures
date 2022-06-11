add_executable(
        d_linked_list_testing_gtest
        d_linked_list_testing_gtest.cpp
        d_linked_list_sort_testing.cpp
)

target_link_libraries(
        d_linked_list_testing_gtest
        PUBLIC
        dl_list
)

include(GTestConfig)
AddTest(d_linked_list_testing_gtest)
