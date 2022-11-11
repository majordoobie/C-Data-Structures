include(BuildUtils)

add_library(utils SHARED utils.c)
set_project_properties(utils "")

add_library(dl_list SHARED dlist_adt/dl_list.c dlist_adt/dl_iter.c)
set_project_properties(dl_list "" utils)