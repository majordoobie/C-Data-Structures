#ifndef DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_
#define DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_
#include <dl_list.h>
#include <bits/stdint-intn.h>

typedef enum
{
    NEXT,
    PREV
} iter_fetch_t;

dlist_iter_t * iter_get_iterable(dnode_t * node, dlist_t * dlist, int32_t index);
int32_t iter_get_iter_index(dlist_iter_t * iter);
dlist_t * iter_get_dlist(dlist_iter_t * iter);
dnode_t * iter_get_iter_node(dlist_iter_t * iter);

void iter_set_iter_node(dlist_iter_t * iter, dnode_t * node, int32_t index);
dnode_t * iterate(dlist_iter_t * iter, iter_fetch_t fetch);
#endif //DATA_STRUCTURES_C_DATA_STRUCTURES_DLIST_ADT_SRC_DL_ITER_H_
