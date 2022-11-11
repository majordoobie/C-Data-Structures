#[=[
    Set boiler settings that exist in all projects and not just unique
    to the current project
#]=]
FUNCTION(SetBoilerSettings)
    # Set Langauge standards
    set(CMAKE_C_STANDARD 17)
    set(CMAKE_CXX_STANDARD 20)

    # Perform static analysis with clang-tidy if package is found
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}" "-checks=-*,readability-*" CACHE STRING "" FORCE)
    ELSEIF(NOT CLANG_TIDY_EXE)
        message(WARNING "Clan-Tidy not found. Will not apply static analysis")
    ENDIF()

    # Warn to user that clang-format is not installed
    find_program(CLANG-FORMAT_PATH clang-format)
    IF(NOT CLANG-FORMAT_PATH)
        message(WARNING "No clang-format binary found. Will not auto-format.")
    ENDIF()

ENDFUNCTION()


#[=[
    Function applies the `.clang-tidy` format by calling clang-tidy if the
    binary is found.

    param target: Target name provided
#]=]
FUNCTION(Format target)
    find_program(CLANG-FORMAT_PATH clang-format)
    IF(CLANG-FORMAT_PATH)

        # Create regex strings with the src and include dirs
        set(SRC_REGEX h hpp hh c cc cxx cpp)
        set(INCLUDE_REGEX h hpp hh c cc cxx cpp)
        list(TRANSFORM SRC_REGEX PREPEND "${PROJECT_SOURCE_DIR}/src/*.")
        list(TRANSFORM INCLUDE_REGEX PREPEND "${PROJECT_SOURCE_DIR}/include/*.")

        # Glob for the regex files
        file(GLOB_RECURSE SOURCE_FILES FOLLOW_SYMLINKS
                LIST_DIRECTORIES false ${SRC_REGEX} ${INCLUDE_REGEX}
                )

        # Apply clang-tidy to the source files
        add_custom_command(TARGET ${target} PRE_BUILD COMMAND
                ${CLANG-FORMAT_PATH} -i --style=file ${SOURCE_FILES}
                )
    ENDIF()

ENDFUNCTION()

#[=[
    Macro is used to set variables for compiler flags.
    Macros scope populates inside the scope of the caller
    -D_FORITIFY_SOURCE=2 : Detect runtime buffer overflow
    -fpie, -Wl,-pie : full ASLR
    -fpic -shared : Disable text relocations for shared libraries
#]=]
MACRO(SetCompilerFlags)
    # base flags for detecting errors
    set(base_exceptions
            "-Wall"
            "-Wshadow"
            "-Wconversion"
            "-Wpedantic"
            "-Wformat"
            "-Wvla"
            "-Wfloat-equal"
            "-D_FORTIFY_SOURCE=2"
            "-fpie"
            "-Wl,-pie"
            "-fPIC"
            )

    # Base flags for static analysis. This should be added to both the
    # compiler and linker options
    set(base_static_analysis
            "-fsanitize=address"
            "-fno-omit-frame-pointer"
            "-fsanitize=undefined"
            "-fno-sanitize-recover=all"
            "-fsanitize=float-divide-by-zero"
            "-fsanitize=float-cast-overflow"
            "-fno-sanitize=null"
            "-fno-sanitize=alignment"
    )


    # Add debuging symbols if in debug mode
    IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(debug_flag "-g3")
    ENDIF()

    # Create a base flags variable to be linked
    set(base_flags ${base_exceptions} ${base_static_analysis} ${debug_flag})
ENDMACRO()


#[=[
    set_project_properties the include directory using the relocatable generator
    syntax which allows the --prefix syntax to be used without breaking where
    the header files are located. Additionally, the function sets the compiler
    flags for the target.

    Param target_name: Name of the target to apply the configurations to

    Param include_directory: Additional include directory that is not
    ${PROJECT_SOURCE_DIR}/include

    Param ARGN: Every other variable passed in will be used as targets
    to link
#]=]
FUNCTION(set_project_properties target_name include_directory)
    # Run macro to get the variables set
    SetCompilerFlags()
    Format(${target_name})

    # Separates the area of concern when it comes to the build files and the
    # installation files by using the build in generator expression for relocation
    target_include_directories(
            ${target_name} PUBLIC
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
            "$<BUILD_INTERFACE:${include_directory}>"
            "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    # set additional target properties to include flags for static analysis
    set_target_properties(
            ${target_name} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
            COMPILE_OPTIONS "${base_flags}"
            LINK_OPTIONS "${base_static_analysis}"
            CXX_CLANG_TIDY "$CACHE{CLANG_TIDY_COMMAND}"
            C_CLANG_TIDY "$CACHE{CLANG_TIDY_COMMAND}"
    )
    target_link_libraries(${target_name} ${ARGN})

ENDFUNCTION()



#[=[
    GTest_add_target sets the build path and flags for the gtest executable
    and places the result in the ${CMAKE_BINARY_DIR}/test_bin for easy retrival
#]=]
FUNCTION(GTest_add_target target_name)
    # Include populates the scope of the caller
    include(GoogleTest)

    # run the flags macro
    SetCompilerFlags()

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
