#ifndef DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_
#define DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <dl_list.h>
#include <bits/stdint-intn.h>

typedef enum
{
    NEXT,
    PREV
} iter_fetch_t;

typedef enum
{
    SEARCH_BY_INDEX,
    SEARCH_BY_VALUE,
} iter_search_by;

typedef enum
{
    SEARCH_SUCCESS,
    SEARCH_FAILURE
} iter_search_result;

typedef struct
{
    dlist_iter_t * iter;
    void * target_data;
    int32_t target_index;
    dnode_t * found_node;
    int32_t found_index;
    iter_search_by search_by;
} iter_search_t;

// Create an iterable
dlist_iter_t * iter_get_iterable(dnode_t * node,
                                 dlist_t * dlist,
                                 int32_t index);
void iter_destroy_iterable(dlist_iter_t * iter);

// Get iter values
int32_t iter_get_index(dlist_iter_t * iter);
dnode_t * iter_get_node(dlist_iter_t * iter);
dlist_t * iter_get_dlist(dlist_iter_t * iter);
void iter_update_index(dlist_iter_t * iter, int index);

// Search for values
iter_search_t * iter_search_by_value_plus(dlist_t * dlist, void * data);
iter_search_result iter_search(iter_search_t * search);
dnode_t * iter_search_by_value(dlist_t * dlist, void * data);
dnode_t * iter_search_by_index(dlist_t * dlist, int32_t index);
void iter_destroy_search(iter_search_t * search);
iter_search_t * iter_init_search(dlist_iter_t * iter,
                                 void * data,
                                 int32_t index,
                                 iter_search_by search_by);

// Modify the iter pointer
void iter_set_node(dlist_iter_t * iter, dnode_t * node, int32_t index);
dnode_t * iterate(dlist_iter_t * iter, iter_fetch_t fetch);

#ifdef __cplusplus
}
#endif
#endif //DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_
