add_executable(
        queue_testing_gtest
        queue_testing_gtest.cpp
)

target_link_libraries(
        queue_testing_gtest
        PUBLIC
        dl_queue
)

include(GTestConfig)
AddTest(queue_testing_gtest)
