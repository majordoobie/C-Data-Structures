add_library(l_list SHARED l_list.c)

# Test to public so that gtest can have access
target_include_directories(
        l_list
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
