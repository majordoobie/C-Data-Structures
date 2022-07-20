add_executable(
        heap_testing_gtest
        heap_adt_gtest.cpp
)

target_link_libraries(
        heap_testing_gtest
        PUBLIC
        heap
)

include(BuildUtils)
GTest_add_target(heap_testing_gtest)
