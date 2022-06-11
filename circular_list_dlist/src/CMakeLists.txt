add_library(circular_list SHARED circular_list.c)

# Test to public so that gtest can have access
target_include_directories(
        circular_list
        PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
        circular_list
        dl_list
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(circular_list)
