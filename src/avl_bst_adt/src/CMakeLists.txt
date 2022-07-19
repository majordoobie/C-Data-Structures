add_library(bst SHARED bst.c)

message(FATAL_ERROR "What is the ucrrent source now? ${CMAKE_CURRENT_SOURCE_DIR}")

# Test to public so that gtest can have access
target_include_directories(
        bst
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(bst)