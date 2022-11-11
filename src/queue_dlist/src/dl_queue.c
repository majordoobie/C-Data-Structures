#include <assert.h>
#include <dl_list.h>
#include <dl_queue.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct queue_t {
  dlist_t *dlist;
  size_t queue_size;
} queue_t;

/*!
 * @brief Initialize the queue structure. A NULL is returned if there was a
 * failure in the creation of the structure
 * @param queue_size
 * @param compare_func
 * @return
 */
queue_t *queue_init(size_t queue_size,
                    queue_status_t (*compare_func)(void *, void *)) {
  // dlist will abort if it cannot allocate
  dlist_t *dlist = dlist_init((dlist_match_t(*)(void *, void *))compare_func);

  queue_t *queue = (queue_t *)malloc(sizeof(queue_t));
  if (NULL == queue) {
    dlist_destroy(dlist);
    fprintf(stderr, "[!] Invalid allocation\n");
    return NULL;
  }
  *queue = (queue_t){.dlist = dlist, .queue_size = queue_size};

  return queue;
}

/*!
 * @brief Frees the structure while leaving the data in the queue intact
 * @param queue
 */
void queue_destroy(queue_t *queue) { queue_destroy_free(queue, NULL); }

/*!
 * @brief Frees the structure and frees the items in the queue with the
 * function pointer passed in to free them.
 * @param queue
 * @param free_func
 */
void queue_destroy_free(queue_t *queue, void (*free_func)(void *data)) {
  assert(queue);
  if (NULL != free_func) {
    dlist_destroy_free(queue->dlist, free_func);
  } else {
    dlist_destroy(queue->dlist);
  }
  free(queue);
}

/*!
 * @brief Return the amount of items in the queue
 * @param queue
 * @return
 */
size_t queue_length(queue_t *queue) { return dlist_get_length(queue->dlist); }

/*!
 * @brief Return bool indicating if the queue is empty
 * @param queue
 * @return
 */
bool queue_is_empty(queue_t *queue) { return (queue_length(queue) == 0); }

/*!
 * @brief Add an item to the queue and return a status indicating if it was
 * successful or not. A failure indicates that the queue is already full and
 * cannot enqueue anymore.
 * @param queue
 * @param data
 * @return
 */
queue_status_t queue_enqueue(queue_t *queue, void *data) {
  if (queue_length(queue) == queue->queue_size) {
    return Q_FAILURE;
  }

  dlist_append(queue->dlist, data);
  return Q_SUCCESS;
}

/*!
 * @brief Pop an item from the queue. If the queue is empty, return a NULL item.
 * @param queue
 * @return
 */
void *queue_dequeue(queue_t *queue) {
  if (queue_is_empty(queue)) {
    return NULL;
  }
  return dlist_pop_head(queue->dlist);
}

/*!
 * @brief Fetch a value from the queue based on the data using the comparison
 * function.
 * @param queue
 * @param data
 * @return
 */
queue_t *queue_get_by_value(queue_t *queue, void *data) {
  return dlist_get_by_value(queue->dlist, data);
}

/*!
 * @brief Fetch a value from the queue based on its index
 * @param queue
 * @param index
 * @return
 */
queue_t *queue_get_by_index(queue_t *queue, size_t index) {
  return dlist_get_by_index(queue->dlist, (int)index);
}

/*!
 * @brief Remove the specified node from the queue
 * @param queue
 * @param data
 * @return NULL if node does not exist. Otherwise, the pointer to the node
 * data is returned.
 */
void *queue_remove(queue_t *queue, void *data) {
  assert(queue);
  assert(data);

  return dlist_remove_value(queue->dlist, data);
}
