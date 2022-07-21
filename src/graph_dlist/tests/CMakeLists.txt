add_executable(
        graph_dlist_testing_gtest
        graph_dlist_testing_gtest.cpp
)

target_link_libraries(
        graph_dlist_testing_gtest
        PUBLIC
        graph_dlist
)

include(BuildUtils)
GTest_add_target(graph_dlist_testing_gtest)
