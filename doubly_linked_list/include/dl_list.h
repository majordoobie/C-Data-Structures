#ifndef DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#define DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stdbool.h>
typedef enum
{
    DLIST_MATCH,
    DLIST_MISS_MATCH
} dlist_match_t;

typedef struct dlist_t dlist_t;
typedef struct dlist_iter_t dlist_iter_t;
dlist_t * dlist_init(dlist_match_t (* compare_func)(void *, void *));
void dlist_destroy(dlist_t * dlist);
void dlist_append(dlist_t * dlist, void * data);
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *));

// Fetching values
void * dlist_pop_tail(dlist_t * dlist);
void * dlist_pop_head(dlist_t * dlist);
void * dlist_get_value(dlist_t * dlist, void * data);
void * dlist_remove_value(dlist_t * dlist, void * data);
bool dlist_is_empty(dlist_t * dlist);


// Find values
dlist_match_t dlist_in_dlist(dlist_t * dlist, void * data);

// iterables
dlist_iter_t * dlist_get_iterable(dlist_t * dlist);
void * dlist_get_iter_next(dlist_iter_t * dlist_iter);
void dlist_destroy_iter(dlist_iter_t * dlist_iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
