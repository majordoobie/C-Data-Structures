include_directories(${CMAKE_SOURCE_DIR}/include)

add_library(intersect_cli_lib arg_parser.c file_parser.c)