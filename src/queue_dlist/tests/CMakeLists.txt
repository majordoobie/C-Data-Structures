add_executable(
        queue_testing_gtest
        queue_dlist_gtest.cpp
)

target_link_libraries(
        queue_testing_gtest
        PUBLIC
        dl_queue
)

include(BuildUtils)
GTest_add_target(queue_testing_gtest)
