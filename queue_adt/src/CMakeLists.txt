add_library(queue SHARED queue.c)

# Test to public so that gtest can have access
target_include_directories(
        queue
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
