add_library(hashtable SHARED heap.c)

# Test to public so that gtest can have access
target_include_directories(
        hashtable
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(hashtable)
