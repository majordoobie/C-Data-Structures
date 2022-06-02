#ifndef DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#define DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stdbool.h>

typedef struct dlist_t dlist_t;
typedef struct dlist_iter_t dlist_iter_t;
dlist_t * dlist_init();
//void * get_value(dlist_t * dlist, int32_t index);
void dlist_destroy(dlist_t * dlist);
void dlist_append(dlist_t * dlist, void * data);
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *));

// iterables
dlist_iter_t * dlist_get_iterable(dlist_t * dlist);
bool dlist_iter_is_empty(dlist_iter_t * dlist_iter);
void * dlist_get_iter_next(dlist_iter_t * dlist_iter);
void dlist_destroy_iter(dlist_iter_t * dlist_iter);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
