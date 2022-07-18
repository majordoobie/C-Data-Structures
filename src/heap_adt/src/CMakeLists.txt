add_library(heap SHARED heap.c)

# Test to public so that gtest can have access
target_include_directories(
        heap
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(heap)