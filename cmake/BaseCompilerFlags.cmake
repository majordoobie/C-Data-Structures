function(AddFlags target)
    target_compile_options(
            ${target}
            PUBLIC
            -Wall
            -Wextra
            -Wpedantic
            -Wformat
            -Wvla
            -Wfloat-equal
            -Werror
            -D_FORTIFY_SOURCE=2
            -fpie
            -Wl,-pie
            -fpic
            -shared
    )
endfunction()