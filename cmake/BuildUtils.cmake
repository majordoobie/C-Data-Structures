# Macro is used to set variables for compiler flags.
# Macros scope populates inside the scope of the caller
MACRO(set_compiler_flags)
    # base flags for detecting errors
#    set(base_exceptions "-Wall;-Wshadow;-Werror;-Wconversion;-Wpedantic;-Wformat;-Wvla;-Wfloat-equal;-D_FORTIFY_SOURCE=2;-fpie;-Wl,-pie;-shared")
    set(base_exceptions "-Wall"
            "-Wshadow"
            "-Werror"
            "-Wconversion"
            "-Wpedantic"
            "-Wformat"
            "-Wvla"
            "-Wfloat-equal"
            "-D_FORTIFY_SOURCE=2"
            "-fpie"
            "-Wl,-pie"
            "-shared")

    # Base flags for static analysis. This should be added to both the
    # compiler and linker options
    set(base_static_analysis "-fno-omit-frame-pointer;-fsanitize=address;-fsanitize=undefined;-fno-sanitize-recover=all;-fsanitize=float-divide-by-zero;-fsanitize=float-cast-overflow;-fno-sanitize=null;-fno-sanitize=alignment")


    # Add debuging symbols if in debug mode
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(debug_flag "-g3")
    ENDIF()

    # Create a base flags variable to be linked
    set(base_flags ${base_exceptions} ${base_static_analysis} ${debug_flag})

ENDMACRO()


#
# set_project_properties the include directory using the relocatable generator
# syntax which allows the --prefix syntax to be used without breaking where
# the header files are located. Additionally, the function sets the compiler
# flags for the target
#
FUNCTION(set_project_properties target_name target_include_dir)
    # Run macro to get the variables set
    set_compiler_flags()

    # Separates the area of concern when it comes to the build files and the
    # installation files by using the build in generator expression for relocation
    target_include_directories(
            ${target_name} PUBLIC
            "$<BUILD_INTERFACE:${target_include_dir}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    # set additional target properties to include flags for static analysis
    set_target_properties(
            ${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            COMPILE_OPTIONS "${base_flags}"
            LINK_OPTIONS "${base_static_analysis}"
    )
ENDFUNCTION()



#
# GTest_add_target sets the build path and flags for the gtest executable
# and places the result in the ${CMAKE_BINARY_DIR}/test_bin for easy retrival
#
FUNCTION(GTest_add_target target_name)
    # Include populates the scope of the caller
    include(GoogleTest)

    # run the flags macro
    set_compiler_flags()

    # Set flags and set the output of the binaries to a the test_bin
    set_target_properties(
            ${target_name} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test_bin
            COMPILE_OPTIONS "${base_flags}"
            LINK_OPTIONS "${base_static_analysis}"
    )
    target_link_libraries(${target_name} PRIVATE gtest_main)
    gtest_discover_tests(${target_name})

ENDFUNCTION()