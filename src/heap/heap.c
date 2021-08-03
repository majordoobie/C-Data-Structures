#include <heap.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
    BASE_SIZE = 5,
} heap_default_t;

static void ensure_space(heap_t * heap);


/*!
 * @brief Create the basic heap structure with default array size of 10 blocks
 * @param compare[in] Compare function used on each heap payload
 * @param destroy[in] Function called on each payload when freeing
 * @return Heap pointer
 */
heap_t * init_heap(int (* compare)(heap_payload_t * payload, heap_payload_t * payload2), void (* destroy)(heap_payload_t * payload))
{
    heap_t * heap = calloc(1, sizeof(* heap));
    heap->compare = compare;
    heap->destroy = destroy;
    heap->heap_size = BASE_SIZE;
    heap->heap_array = calloc(heap->heap_size, sizeof(heap->heap_array));
    return heap;
}

/*!
 * @brief Free the heap structure
 * @param heap[in] Allocated heap pointer
 */
void destroy_heap(heap_t * heap)
{
    for (int i = 0; i < heap->length; i++)
    {
        heap->destroy(heap->heap_array[i]);
    }
    free(heap->heap_array);
    free(heap);
}

void insert_heap(heap_t * heap, heap_payload_t * payload)
{
    ensure_space(heap);

    heap->heap_array[heap->length] = payload;
    heap->length++;
}

static void ensure_space(heap_t * heap)
{
    if (heap->length == heap->heap_size)
    {
        heap->heap_size = heap->heap_size * 2;
        heap_payload_t ** re_alloc = realloc(heap->heap_array, sizeof(heap->heap_array) * heap->heap_size);
        if (NULL == re_alloc)
        {
            fprintf(stderr, "Could not reallocate memory for heap!");
            abort();
        }
        heap->heap_array =  re_alloc;
    }
}
