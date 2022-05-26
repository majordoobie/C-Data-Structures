#include <heap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    BASE_SIZE = 5,
} heap_default_t;

// Enum for determining if malloc calls were valid
typedef enum
{
    VALID_PTR = 1,
    INVALID_PTR = 0
} heap_pointer_t;


typedef struct heap_t
{
    size_t array_length;            // Number of active nodes in the array
    size_t array_size;              // Physical size of the array
    size_t node_size;               // Size of each node in the array
    heap_data_mode_t data_mode;     // Mode being pointer mode or data mode

    heap_compare_t heap_type;
    void ** heap_array;
    heap_compare_t (* compare)(void * payload, void * payload2);
    void (* destroy)(void * payload);
} heap_t;

static void ensure_space(heap_t * heap);
static void ensure_downgrade_size(heap_t * heap);
static void resize_heap(heap_t *heap);

static void bubble_up(heap_t * heap);
static void bubble_down(heap_t * heap);
static void swap(heap_t * heap, size_t child, size_t parent);

static size_t parent_index(size_t index);
static size_t left_child_index(size_t index);
static size_t right_child_index(size_t index);

static bool is_valid_parent(heap_t * heap, size_t index);
static bool has_left_child(heap_t * heap, size_t index);
static bool has_right_child(heap_t * heap, size_t index);

static size_t get_largest_child_index(heap_t * heap, size_t index);
static void * get_left_child(heap_t * heap, size_t index);
static void * get_right_child(heap_t * heap, size_t index);

static heap_pointer_t verify_alloc(void * ptr);



/*!
 * @brief Test function for printing out the arrays in order
 * @param heap[in] heap_t
 * @param print_test[in] pointer to the print function for printing the "heap_payload_t"
 */
void heap_print(heap_t * heap, void (*print_test)(void *payload))
{
    for (size_t i = 0; i < heap->array_length; i++)
    {
        print_test(heap->heap_array[i]);
    }
}

/*!
 * @brief Create the initial data structure for the heap.
 *
 * The heap data structure is an array that follows the rules of a binary tree.
 * But the data itself is stored in a array. The array can either be an array
 * of void pointers or an array of data.
 *
 * The mode is selected by the data_mode parameter. HEAP_PTR creates an array
 * of void pointers while HEAP_MEM creates an array of the memory blocks
 * @param type Heap type, max heap or min heap
 * @param data_mode Data storage strategy
 * @param payload_size The size of the payload. This can be 0 if using HEAP_PTR
 * @param destroy Pointer to function that frees the block of memory
 * @param compare Pointer to function that compares the nodes
 * @return Pointer to heap or NULL
 */
heap_t * heap_init(heap_type_t type,
                   heap_data_mode_t data_mode,
                   size_t payload_size,
                   void (* destroy)(void *),
                   heap_compare_t (* compare)(void *, void *))
{
    // Allocate the space needed for creating the base structure
    heap_t * heap = (heap_t *)malloc(sizeof(heap_t));
    if (INVALID_PTR == verify_alloc((void *)heap))
    {
        return NULL;
    }


    *heap = (heap_t){
        // Set sizes
        .array_length       = 0,
        .array_size         = BASE_SIZE,
        .node_size          = payload_size,

        // Set heap type
        .heap_type          = type ? HEAP_LT : HEAP_GT,
        .data_mode          = data_mode,

        // Set heap array
        .heap_array         = NULL,

        // Set callback functions
        .compare            = compare,
        .destroy            = destroy
    };

    if (heap->data_mode == HEAP_PTR)
    {
        // if mode is pointer, then just create an array to hold the pointers
        heap->heap_array = calloc(heap->array_size, sizeof(void *));
    }
    else
    {
        // If in data mode, then create the space for the data itself
        heap->heap_array = calloc(heap->array_size, heap->node_size);
    }

    // Verify that the array was created successfully
    if (INVALID_PTR == verify_alloc(heap->heap_array))
    {
        free(heap);
        return NULL;
    }
    return heap;
}

/*!
 * @brief Insert a payload into the heap
 * @param heap[in] heap_t
 * @param payload[in] heap_payload_t
 */
void heap_insert(heap_t * heap, void * payload)
{
    // checks to make sure we have enough space
    ensure_space(heap);

    if (heap->data_mode == HEAP_PTR)
    {
        // adds the payload to the array
        heap->heap_array[heap->array_length] = payload;
    }
    else
    {
        memcpy(heap->heap_array[heap->array_length], payload, heap->node_size);
    }

    // increment the array_length of the array
    heap->array_length++;

    // perform bubble up
    bubble_up(heap);
}

/*!
 * @brief Free the heap structure
 * @param heap[in] Allocated heap pointer
 */
void heap_destroy(heap_t * heap)
{
    // If the mode is set to ptr, then free all the elements
    // but if it is not, then we do not need to free it
    if (heap->data_mode == HEAP_PTR)
    {
        for(size_t i = 0; i < heap->array_length; i++)
        {
            if (NULL != heap->destroy)
            {
                heap->destroy(heap->heap_array[i]);
            }
        }
    }

    free(heap->heap_array);
    free(heap);
}

void heap_sort(void ** array, size_t item_count, heap_compare_t (*compare)(void *, void *), heap_type_t type)
{
    heap_t * heap = heap_init(type, HEAP_PTR, 0, NULL, compare);

    // heapify the array given
    for (size_t i = 0; i < item_count; i++)
    {
        heap_insert(heap, array[i]);
    }

    // modify the array given in place with heapsort
    for (size_t i = 0; i < item_count; i++)
    {
        array[i] = heap_pop(heap);
    }
    heap_destroy(heap);
}

//heap_payload_t * heap_peek(heap_t * heap, int index)
//{
//    ;;
//}

void heap_dump(heap_t * heap)
{
    for (size_t i = 0; i < heap->array_length; i++)
    {
        heap->destroy(heap->heap_array[i]);
    }
    heap->array_length = 0;
    ensure_downgrade_size(heap);
}


/*!
 * @brief Check if the heap is currently empty
 * @param heap[in]
 * @return
 */
bool heap_is_empty(heap_t * heap)
{
    return (heap->array_length == 0);
}


/*!
 * @brief Pop the next node in the heap, this should be the current highest node. The
 * array is resized if it needs to be to avoid taking up unnecessary space.
 *
 * This operation runs at O(lon n) because it relies on the bubble operations
 * to restructure the tree
 *
 * @param heap[in] heap_t
 * @return heap_payload_t
 */
void * heap_pop(heap_t * heap)
{
    // check if heap is empty, if so, return
    if (heap_is_empty(heap))
    {
        return NULL;
    }

    // pop the root node and decrement our array_length size
    void * removal_payload = heap->heap_array[0];
    heap->array_length--;
    heap->heap_array[0] = heap->heap_array[heap->array_length];

    // perform the bubble down algorithm
    bubble_down(heap);

    // resize array if we need to
    ensure_downgrade_size(heap);

    // return pop value
    return removal_payload;
}

/*!
 * @brief Checks to see if the array needs to be increased
 * @param heap[in] heap_t
 */
static void ensure_space(heap_t * heap)
{
    if (heap->array_length == heap->array_size)
    {
        heap->array_size = heap->array_size * 2;
        resize_heap(heap);
    }
}

/*!
 * @brief Checks to see if the array can be decreased to save space
 * @param heap[in] heap_t
 */
static void ensure_downgrade_size(heap_t * heap)
{
    if ((heap->array_length == (heap->array_size / 2)) & (heap->array_size > BASE_SIZE))
    {
        heap->array_size = heap->array_size / 2;
        resize_heap(heap);
    }
    if ((heap->array_length == 0) & (heap->array_size == BASE_SIZE))
    {
        resize_heap(heap);
    }
}

/*!
 * @brief Resizes heap based on the new array_size value
 * @param heap[in] heap_t
 */
static void resize_heap(heap_t *heap)
{
    void ** re_alloc = realloc(heap->heap_array, sizeof(heap->heap_array) *
    heap->array_size);
    if (NULL == re_alloc)
    {
        fprintf(stderr, "Could not reallocate memory for heap!");
        abort();
    }
    heap->heap_array =  re_alloc;
}

/*!
 * @brief Bubble up operations are performed on nodes that are of greater
 * value than their parents. The operation is performed until the node
 * has a parent that is of equal or less value
 *
 * This operation only occurs at maximum of the height of the tree making
 * it have a time complexity of O(log n)
 * @param heap
 */
static void bubble_up(heap_t * heap)
{
    // get the last index
    size_t index = heap->array_length - 1;

    // make sure that it's in the correct position
    while ((index > 0) && (heap->compare(heap->heap_array[index], heap->heap_array[parent_index(index)]) == heap->heap_type))
    {
        swap(heap, index, parent_index(index));
        index = parent_index(index);
    }
}

/*!
 * @brief Bubble down operations ar performed on nodes that are lesser
 * than their parents. This operation is performed until the node
 * has parents that are equal or greater value
 *
 * This operation only occurs at maximum of the height of the tree making
 * it have a time complexity of O(log n)
 *
 * @param heap[in]
 */
static void bubble_down(heap_t * heap)
{
    size_t index = 0;
    while ((index <= heap->array_length) && (!(is_valid_parent(heap, index))))
    {
        size_t largest_index = get_largest_child_index(heap, index);
        swap(heap, index, largest_index);
        index = largest_index;
    }
}

/*!
 * @brief Gets the parent index of the index provided
 * @param index[in] index to inspect
 * @return Index of the parent
 */
static size_t parent_index(size_t index)
{
    return (index - 1) / 2;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static size_t left_child_index(size_t index)
{
    return index * 2 + 1;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static size_t right_child_index(size_t index)
{
    return index * 2 + 2;
}

/*!
 * @brief Swap the positions of the two index in the array
 * @param heap[in] heap_t
 * @param child[in] index of the child to swap from
 * @param parent[in] index of the new parent
 */
static void swap(heap_t * heap, size_t child, size_t parent)
{
    void * temp_payload = heap->heap_array[child];
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
static bool is_valid_parent(heap_t * heap, size_t index)
{
    // If there is no left child, then the parent IS left_compare because there are no other children
    if (!(has_left_child(heap, index)))
    {
        return true;
    }

    // since left child exists, check if current item is greater or less than
    heap_compare_t left_compare = heap->compare(heap->heap_array[index], get_left_child(heap, index));

    // If there is no right child, check to see if the left child makes the parent valid or not
    if (!(has_right_child(heap, index)))
    {
        // if compare DOES NOT equal the type set return false. This will return true for EQ
        if (heap->heap_type != left_compare)
        {
            return false;
        }
        else
        {
            return true;
        }
    }


    /*
     * If we get here then the current node has both a left and right child. Check that
     * the current node is greater/less than both
     */
    heap_compare_t right_compare = heap->compare(heap->heap_array[index], get_right_child(heap, index));

    if ((heap->heap_type != left_compare) || ( heap->heap_type != right_compare))
    {
        return false;
    }
    else
    {
        return true;
    }
}

static bool has_left_child(heap_t * heap, size_t index)
{
    return left_child_index(index) <= heap->array_length;
}
static bool has_right_child(heap_t * heap, size_t index)
{
    return right_child_index(index) <= heap->array_length;
}

/*!
 * @brief Return the largest index, either the left child, or right child
 * @param heap[in] heap_t
 * @param index[in] index to inspect
 * @return Index of the largest child of the inspected parent
 */
static size_t get_largest_child_index(heap_t * heap, size_t index)
{
    // If there is no left child, then there is no right. Root is the largest child
    if (!(has_left_child(heap, index)))
    {
        return index;
    }

    // If there is no right child, then there is a left child, return left child
    if (!(has_right_child(heap, index)))
    {
        return left_child_index(index);
    }

    // compare which children are the biggest/smallest
    if (heap->compare(get_left_child(heap, index), get_right_child(heap, index)) == heap->heap_type)
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
static void * get_left_child(heap_t * heap, size_t index)
{
    return heap->heap_array[left_child_index(index)];

}

/*!
 * @brief Returns the payload of the right child of the passed in index
 * @param heap[in] heap_t
 * @param index[in] index to inspect
 * @return Heap payload from the returned index
 */
static void * get_right_child(heap_t * heap, size_t index)
{
    return heap->heap_array[right_child_index(index)];
}

/*!
 * Function verifies the alloc, if the alloc fails abort as this could
 * mean that the system has ran out of memory.
 *
 * @param ptr Any allocated pointer
 */
static heap_pointer_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        fprintf(stderr, "[!] Could not allocate memory!\n");
        return INVALID_PTR;
    }
    return VALID_PTR;
}
