#ifndef DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_
#define DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
typedef struct clist_t clist_t;
typedef enum
{
    ASCENDING,
    DESCENDING
} sort_order_t;

typedef enum
{
    CLIST_MATCH = 0,
    CLIST_MISS_MATCH = 1
} clist_match_t;

typedef enum
{
    HEAD,
    TAIL,
    INDEX
} clist_location_t;


typedef enum
{
    FREE_NODES_TRUE,
    FREE_NODES_FALSE
} clist_delete_t;


clist_t * clist_init(uint32_t list_size, clist_match_t (* compare_func)(void *, void *), void (* free_func)(void *));
clist_t * clist_destroy(clist_t * clist, clist_delete_t remove_nodes);
void * clist_get_value(clist_t * clist);
void * clist_get_next(clist_t * clist);
void * clist_find(clist_t * clist, void * node);
void * clist_remove(clist_t * clist, void * node);
void clist_quick_sort(clist_t * clist, sort_order_t order);
void clist_insert(clist_t * clist, void * node, int32_t index, clist_location_t insert_at);


#ifdef __cplusplus
};

#endif
#endif //DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_
