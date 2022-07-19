include(BuildUtils)
#set_custom_properties(dl_list ${CMAKE_CURRENT_SOURCE_DIR})

add_library(dl_list SHARED dl_list.c dl_iter.c)

target_include_directories(
        dl_list PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
        dl_list PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# Add base flags
include(BaseCompilerFlags)
AddFlags(dl_list)
