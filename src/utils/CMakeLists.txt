include(BuildUtils)

add_library(utils SHARED utils.c)
set_project_properties(utils ${CMAKE_CURRENT_SOURCE_DIR})
