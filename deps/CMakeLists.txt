# File contains all the deps for the project to build properly

# The FetchContent module is capable of fetching git repositories and handling
# the cloning of them so you do not have to deal with them
include(FetchContent)

# Only get the gtest if we are in debug mode otherwise just skip
IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Fetch the gtest framework and make it available
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.11.0
    )
    # GMock does not work in C only C++ so just disable
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

    # Make available then include
    FetchContent_MakeAvailable(googletest)
ENDIF()
