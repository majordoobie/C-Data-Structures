#ifndef DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#define DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct dlist_t dlist_t;
dlist_t * dlist_init();
//void * get_value(dlist_t * dlist, int32_t index);
void dlist_destroy(dlist_t * dlist);
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *));

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_LINKED_LIST_ADT_SRC_QUEUE_H_
