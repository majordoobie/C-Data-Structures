add_library(dl_queue SHARED dl_queue.c)

# Test to public so that gtest can have access
target_include_directories(
        dl_queue
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
        dl_queue
        dl_list
)
