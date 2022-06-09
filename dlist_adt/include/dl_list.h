#ifndef DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#define DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stdbool.h>
#include <stdint-gcc.h>
typedef enum
{
    DLIST_MATCH = 0,
    DLIST_MISS_MATCH = 1
} dlist_match_t;
typedef enum
{
    ITER_HEAD,
    ITER_TAIL
} iter_start_t;



typedef struct dlist_t dlist_t;
typedef struct dlist_iter_t dlist_iter_t;

// constructors and descriptors
dlist_t * dlist_init(dlist_match_t (* compare_func)(void *, void *));
void dlist_destroy(dlist_t * dlist);
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *));

// inserting methods
void dlist_append(dlist_t * dlist, void * data);
void dlist_prepend(dlist_t * dlist, void * data);

// manipulation methods
void * dlist_pop_tail(dlist_t * dlist);
void * dlist_pop_head(dlist_t * dlist);
void * dlist_get_by_value(dlist_t * dlist, void * data);
void * dlist_get_by_index(dlist_t * dlist, int index);
void * dlist_remove_value(dlist_t * dlist, void * data);

// metadata methods
bool dlist_is_empty(dlist_t * dlist);
size_t dlist_length(dlist_t * dlist);
dlist_match_t dlist_in_dlist(dlist_t * dlist, void * data);

// iterables
dlist_iter_t * dlist_get_iterable(dlist_t * dlist, iter_start_t pos);
void * dlist_get_iter_value(dlist_iter_t * dlist_iter);
void * dlist_get_iter_next(dlist_iter_t * dlist_iter);
void * dlist_get_iter_prev(dlist_iter_t * dlist_iter);
void dlist_set_iter_head(dlist_iter_t * dlist_iter);
void dlist_set_iter_tail(dlist_iter_t * dlist_iter);
int32_t dlist_get_iter_index(dlist_iter_t * dlist_iter);
void dlist_destroy_iter(dlist_iter_t * dlist_iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
