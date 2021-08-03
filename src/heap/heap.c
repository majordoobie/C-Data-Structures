#include <heap.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
    BASE_SIZE = 5,
} heap_default_t;


typedef struct heap_t
{
    int length;
    int heap_size;
    heap_compare_t direction;
    heap_payload_t ** heap_array;
    heap_compare_t (* compare)(heap_payload_t * payload, heap_payload_t * payload2);
    void (* destroy)(heap_payload_t * payload);
} heap_t;

static void ensure_space(heap_t * heap);
static void ensure_downgrade_size(heap_t * heap);
static void
resize_heap(heap_t *heap);

static void bubble_up(heap_t * heap);
static void bubble_down(heap_t * heap);
static void swap(heap_t * heap, int child, int parent);

static int parent_index(int index);
static int left_child_index(int index);
static int right_child_index(int index);

static bool is_valid_parent(heap_t * heap, int index);
static bool has_left_child(heap_t * heap, int index);
static bool has_right_child(heap_t * heap, int index);

static int get_largest_child_index(heap_t * heap, int index);
static heap_payload_t * get_left_child(heap_t * heap, int index);
static heap_payload_t * get_right_child(heap_t * heap, int index);


/*!
 * @brief Test function for printing out the arrays in order
 * @param heap[in] heap_t
 * @param print_test[in] pointer to the print function for printing the "heap_payload_t"
 */
void print_heap(heap_t * heap, void (print_test)(heap_payload_t * payload))
{
    for (int i = 0; i < heap->length; i++)
    {
        print_test(heap->heap_array[i]);
    }
}

/*!
 * @brief Create the basic heap structure with default array size of 10 blocks
 * @param compare[in] Compare function used on each heap payload
 * @param destroy[in] Function called on each payload when freeing
 * @return Heap pointer
 */
heap_t * init_heap(heap_compare_t (*compare)(heap_payload_t *, heap_payload_t *), void (*destroy)(heap_payload_t *), heap_type_t type)
{
    heap_t * heap = malloc(sizeof(* heap));
    heap->compare = compare;
    heap->destroy = destroy;
    heap->heap_size = BASE_SIZE;
    heap->length = 0;
    heap->heap_array = calloc(heap->heap_size, sizeof(heap_payload_t *));
    heap->direction = type ? HEAP_LT : HEAP_GT;
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

/*!
 * @brief Check if the heap is currently empty
 * @param heap[in]
 * @return
 */
bool heap_is_empty(heap_t * heap)
{
    return (heap->length == 0);
}

/*!
 * @brief Insert a payload into the heap
 * @param heap[in] heap_t
 * @param payload[in] heap_payload_t
 */
void insert_heap(heap_t * heap, heap_payload_t * payload)
{
    // checks to make sure we have enough space
    ensure_space(heap);

    // adds the payload to the array
    heap->heap_array[heap->length] = payload;
    heap->length++;

    // perform bubble up
    bubble_up(heap);
}

/*!
 * @brief Pop the next node in the heap, this should be the current highest node. The
 * array is resized if it needs to be to avoid taking up unnecessary space.
 * @param heap[in] heap_t
 * @return heap_payload_t
 */
heap_payload_t * pop_heap(heap_t * heap)
{
    // check if heap is empty, if so, return
    if (heap_is_empty(heap))
    {
        return NULL;
    }

    heap_payload_t * removal_payload = heap->heap_array[0];
    heap->length--;
    heap->heap_array[0] = heap->heap_array[heap->length];

    bubble_down(heap);

    ensure_downgrade_size(heap);
    return removal_payload;
}

/*!
 * @brief Checks to see if the array needs to be increased
 * @param heap[in] heap_t
 */
static void ensure_space(heap_t * heap)
{
    if (heap->length == heap->heap_size)
    {
        heap->heap_size = heap->heap_size * 2;
        resize_heap(heap);
    }
}

/*!
 * @brief Checks to see if the array can be decreased to save space
 * @param heap[in] heap_t
 */
static void ensure_downgrade_size(heap_t * heap)
{
    if (heap->length == (heap->heap_size / 2))
    {
        heap->heap_size = heap->heap_size / 2;
        resize_heap(heap);
    }
}

/*!
 * @brief Resizes heap based on the new heap_size value
 * @param heap[in] heap_t
 */
static void resize_heap(heap_t *heap)
{
    heap_payload_t ** re_alloc = realloc(heap->heap_array, sizeof(heap->heap_array) * heap->heap_size);
    if (NULL == re_alloc)
    {
        fprintf(stderr, "Could not reallocate memory for heap!");
        abort();
    }
    heap->heap_array =  re_alloc;
}

/*!
 * @brief Perform the bubble up function on the nodes to ensure the right order
 * @param heap
 */
static void bubble_up(heap_t * heap)
{
    // get the last index
    int index = heap->length - 1;

    // make sure that it's in the correct position
    while ((index > 0) && (heap->compare(heap->heap_array[index], heap->heap_array[parent_index(index)]) == heap->direction))
    {
        swap(heap, index, parent_index(index));
        index = parent_index(index);
    }
}

/*!
 * @brief Performs a bubble down function on the current root index
 * @param heap[in]
 */
static void bubble_down(heap_t * heap)
{
    int index = 0;
    while ((index <= heap->length) && (!(is_valid_parent(heap, index))))
    {
        int largest_index = get_largest_child_index(heap, index);
        swap(heap, index, largest_index);
        index = largest_index;
    }
}

/*!
 * @brief Gets the parent index of the index provided
 * @param index[in] index to inspect
 * @return Index of the parent
 */
static int parent_index(int index)
{
    return (index - 1) / 2;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static int left_child_index(int index)
{
    return index * 2 + 1;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static int right_child_index(int index)
{
    return index * 2 + 2;
}

/*!
 * @brief Swap the positions of the two index in the array
 * @param heap[in] heap_t
 * @param child[in] index of the child to swap from
 * @param parent[in] index of the new parent
 */
static void swap(heap_t * heap, int child, int parent)
{
    heap_payload_t * temp_payload = heap->heap_array[child];
    heap->heap_array[child] = heap->heap_array[parent];
    heap->heap_array[parent] = temp_payload;
}

/*!
 * @brief Checks to see if the children of the current root is valid by checking if either
 * child is greater than itself.
 * @param heap
 * @param index
 * @return
 */
static bool is_valid_parent(heap_t * heap, int index)
{
    // If there is no left child, then the parent IS left_compare because there are no other children
    if (!(has_left_child(heap, index)))
    {
        return true;
    }

    // since left child exists, check if current item is greater or less than
    heap_compare_t left_compare = heap->compare(heap->heap_array[index], get_left_child(heap, index));

    if (!(has_right_child(heap, index)))
    {

        return !((-(heap->direction)) == left_compare);
        /*
        if (HEAP_LT == left_compare)
        {
            // If current node is less than its left child, this is not a left_compare parent
            return false;
        }
        else
        {
            return true;
        }
        */
    }

    /*
     * If we get here then the current node is both a left and right child. Check that
     * the current node is greater than both
     */
    heap_compare_t right_compare = heap->compare(heap->heap_array[index], get_right_child(heap, index));
    return !( ((-(heap->direction)) == left_compare) || ((-(heap->direction)) == right_compare));
    /*
    if ((HEAP_LT == left_compare) || (HEAP_LT == right_compare))
    {
        return false;
    }
    else
    {
        return true;
    }
    */
}

static bool has_left_child(heap_t * heap, int index)
{
    return left_child_index(index) <= heap->length;
}
static bool has_right_child(heap_t * heap, int index)
{
    return right_child_index(index) <= heap->length;
}

/*!
 * @brief Return the largest index, either the left child, or right child
 * @param heap[in] heap_t
 * @param index[in] index to inspect
 * @return Index of the largest child of the inspected parent
 */
static int get_largest_child_index(heap_t * heap, int index)
{
    if (!(has_left_child(heap, index)))
    {
        return index;
    }

    if (!(has_right_child(heap, index)))
    {
        return left_child_index(index);
    }

    if (heap->compare(get_left_child(heap, index), get_right_child(heap, index)) == heap->direction)
    {
        return left_child_index(index);
    }
    else
    {
        return right_child_index(index);
    }
}

/*!
 * @brief Returns the payload of the left child of the passed in index
 * @param heap[in] heap_t
 * @param index[in] index to inspect
 * @return Heap payload from the returned index
 */
static heap_payload_t * get_left_child(heap_t * heap, int index)
{
    return heap->heap_array[left_child_index(index)];

}

/*!
 * @brief Returns the payload of the right child of the passed in index
 * @param heap[in] heap_t
 * @param index[in] index to inspect
 * @return Heap payload from the returned index
 */
static heap_payload_t * get_right_child(heap_t * heap, int index)
{
    return heap->heap_array[right_child_index(index)];
}
