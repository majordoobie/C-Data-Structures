add_library(stack SHARED stack.c)

# Test to public so that gtest can have access
target_include_directories(
        stack
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(stack)