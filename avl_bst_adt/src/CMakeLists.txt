add_library(bst SHARED bst.c)

# Test to public so that gtest can have access
target_include_directories(
        bst
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)
