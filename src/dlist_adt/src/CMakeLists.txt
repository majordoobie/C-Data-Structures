#add_library(dl_list SHARED dl_list.c dl_iter.c)
#
## Test to public so that gtest can have access
#target_include_directories(
#        dl_list
#        PUBLIC
#        ${CMAKE_CURRENT_SOURCE_DIR}/../include
#)


add_library(dl_list SHARED dl_list.c dl_iter.c)

target_include_directories(
        dl_list PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
        dl_list PROPERTIES
        PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

#include_directories(${PROJECT_SOURCE_DIR}/include)
#target_include_directories(
#        dl_list
#        SYSTEM
#        PUBLIC
#        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
#        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
#)

#$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/dl_iter.h

# Add base flags
include(BaseCompilerFlags)
AddFlags(dl_list)
