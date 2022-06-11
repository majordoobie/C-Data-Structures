add_executable(
        heap_testing_gtest
        heap_testing_gtest.cpp
)

target_link_libraries(
        heap_testing_gtest
        PUBLIC
        heap
)

include(GTestConfig)
AddTest(heap_testing_gtest)
