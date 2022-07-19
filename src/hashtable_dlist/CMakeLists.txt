add_subdirectory(src)
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(tests)
ENDIF()
