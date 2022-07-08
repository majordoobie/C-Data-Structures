add_executable(
        circular_list_gtest
        circular_list_dlist_gtest.cpp
)

target_link_libraries(
        circular_list_gtest
        PUBLIC
        circular_list
)

include(GTestConfig)
AddTest(circular_list_gtest)
