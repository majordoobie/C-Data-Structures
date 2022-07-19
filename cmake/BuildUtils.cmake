macro(set_compiler_flags)
    set(base_flags "-Wall -Wshadow -Werror -Wconversion -Wpedantic -Wformat -Wvla
                    -Wfloat-equal -D_FORTIFY_SOURCE=2 -fpie -Wl,-pie -shared")

endmacro()

function(set_custom_properties target_name target_include_dir)
    set_compiler_flags()
    target_include_directories(
            ${target_name} PUBLIC
            "$<BUILD_INTERFACE:${target_include_dir}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    set_target_properties(
            ${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    )

    target_compile_options(
            ${target_name} PUBLIC
            ${base_flags}
    )

endfunction()