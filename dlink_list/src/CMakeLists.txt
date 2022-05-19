add_library(dlinked_list SHARED dlinked_list.c)

# Test to public so that gtest can have access
target_include_directories(
        dlinked_list
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
