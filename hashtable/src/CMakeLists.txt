add_library(hashtable SHARED hashtable.c)

# Test to public so that gtest can have access
target_include_directories(
        hashtable
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
