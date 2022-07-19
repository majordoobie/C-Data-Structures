add_executable(
        d_linked_list_testing_gtest
        dlist_adt_list_testing_gtest.cpp
        dlist_adt_sort_testing_gtest.cpp
)

target_link_libraries(
        d_linked_list_testing_gtest
        PUBLIC
        dl_list
)

include(BuildUtils)
GTest_add_target(d_linked_list_testing_gtest)
