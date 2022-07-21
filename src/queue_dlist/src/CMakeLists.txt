include(BuildUtils)

add_library(dl_queue SHARED dl_queue.c)
set_project_properties(dl_queue ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(dl_queue PUBLIC dl_list)

IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(../tests ../tests)
ENDIF()