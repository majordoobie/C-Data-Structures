# Include the gtest framework to be able to discover
include(GoogleTest)

# Append the gtest libraries to the target passed in then set sanitation flags
# for detecting memory leaks
function(AddTest target)
    # Include the gtest framework to be able to discover
    include(GoogleTest)

    target_compile_options(
            ${target}
            PUBLIC
            -g3
            -fno-omit-frame-pointer
            -fsanitize=address
            -fsanitize=undefined
            -fno-sanitize-recover=all
            -fsanitize=float-divide-by-zero
            -fsanitize=float-cast-overflow
            -fno-sanitize=null
            -fno-sanitize=alignment
    )
    target_link_options(
            ${target}
            PUBLIC
            -g3
            -fno-omit-frame-pointer
            -fsanitize=address
            -fsanitize=undefined
            -fno-sanitize-recover=all
            -fsanitize=float-divide-by-zero
            -fsanitize=float-cast-overflow
            -fno-sanitize=null
            -fno-sanitize=alignment
    )
    target_link_libraries(${target} PRIVATE gtest gtest_main)
    gtest_discover_tests(${target})
endfunction()