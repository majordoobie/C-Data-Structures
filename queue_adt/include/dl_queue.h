#ifndef DATA_STRUCTURES_C_QUEUE_ADT_INCLUDE_QUEUE_H_
#define DATA_STRUCTURES_C_QUEUE_ADT_INCLUDE_QUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <stddef.h>
#include <stdbool.h>
typedef struct queue_t queue_t;
typedef enum
{
    Q_MATCH = 0,
    Q_NO_MATCH = 1,
    Q_SUCCESS,
    Q_FAILURE
} queue_status_t;

// constructors
queue_t * queue_init(size_t queue_size, queue_status_t (* compare_func)(void*, void *));
void queue_destroy(queue_t * queue);
void queue_destroy_free(queue_t * queue, void (* free_func)(void * data));

// Queue info
size_t queue_length(queue_t * queue);
bool queue_is_empty(queue_t * queue);

queue_status_t queue_enqueue(queue_t * queue, void * data);
void * queue_dequeue(queue_t * queue);

queue_t * queue_get_by_index(queue_t * queue, size_t index);
queue_t * queue_get_by_value(queue_t * queue, void * data);
queue_status_t queue_remove(queue_t * queue, void * data);
void queue_clear(queue_t * queue);


#ifdef __cplusplus
}
#endif // __cplusplus
#endif //DATA_STRUCTURES_C_QUEUE_ADT_INCLUDE_QUEUE_H_
