#include <dl_queue.h>
#include <dl_list.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct queue_t
{
    dlist_t * dlist;
    size_t queue_size;
} queue_t;

/*!
 * @brief Initialize the queue structure. A NULL is returned if there was a
 * failure in the creation of the structure
 * @param queue_size
 * @param compare_func
 * @return
 */
queue_t * queue_init(size_t queue_size, queue_status_t (* compare_func)(void*, void *))
{
    // dlist will abort if it cannot allocate
    dlist_t * dlist = dlist_init((dlist_match_t (*)(void *,void *))compare_func);

    queue_t * queue = (queue_t * )malloc(sizeof(queue_t));
    if (NULL == queue)
    {
        dlist_destroy(dlist);
        fprintf(stderr, "[!] Invalid allocation\n");
        return NULL;
    }
    *queue = (queue_t)
        {
            .dlist      = dlist,
            .queue_size = queue_size
        };

    return queue;
}

/*!
 * @brief Frees the structure while leaving the data in the queue intact
 * @param queue
 */
void queue_destroy(queue_t * queue)
{
    queue_destroy_free(queue, NULL);
}

/*!
 * @brief Frees the structure and frees the items in the queue with the
 * function pointer passed in to free them.
 * @param queue
 * @param free_func
 */
void queue_destroy_free(queue_t * queue, void (* free_func)(void * data))
{
    assert(queue);
    if (NULL != free_func)
    {
        dlist_destroy_free(queue->dlist, free_func);
    }
    else
    {
        dlist_destroy(queue->dlist);
    }
    free(queue);
}

size_t queue_length(queue_t * queue)
{
    return dlist_length(queue->dlist);
}
