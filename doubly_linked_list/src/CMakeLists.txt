add_library(dl_list SHARED dl_list.c)

message(${PROJECT_SOURCE_DIR})
# Test to public so that gtest can have access
target_include_directories(
        dl_list
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
