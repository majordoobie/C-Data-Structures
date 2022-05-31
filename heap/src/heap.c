#include <heap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

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
static void resize_heap(heap_t * heap);

static void bubble_up(heap_t * heap);
static void bubble_down(heap_t * heap);
static void swap(heap_t * heap, size_t child_index, size_t parent_index);

static size_t get_parent_index(size_t index);
static size_t get_left_child_index(size_t index);
static size_t get_right_child_index(size_t index);
static size_t get_target_index(heap_t * heap, size_t parent_index);
static size_t get_index(size_t index, size_t node_size);

static bool is_valid_parent(heap_t * heap, size_t parent_index);
static bool has_left_child(heap_t * heap, size_t index);
static bool has_right_child(heap_t * heap, size_t index);

static uint8_t * get_slice(heap_t * heap, size_t index);
static heap_compare_t get_comparison(heap_t * heap,
                                     size_t left_index,
                                     size_t right_index);

static heap_pointer_t verify_alloc(void * ptr);



/*!
 * @brief Test function for printing out the arrays in order
 * @param heap[in] heap_t
 * @param print_test[in] pointer to the print function for printing the "heap_payload_t"
 */
void heap_print(heap_t * heap, void (* print_test)(void * payload))
{
    assert(heap);
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

    * heap = (heap_t){
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
 * @brief Destroy the data structure. If in PTR mode then
 * free the pointers as well
 * @param heap self
 */
void heap_destroy(heap_t * heap)
{
    // ensure that a valid pointer was passed in
    assert(heap);

    // If the mode is set to ptr, then free all the elements
    // bt if it is not, then we do not need to free it
    if (HEAP_PTR == heap->data_mode)
    {
        for (size_t i = 0; i < heap->array_length; i++)
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

/*!
 * @brief insert payload into the heap
 *
 * @param heap heap data structure
 * @param payload Pointer to the payload passed in
 */
void heap_insert(heap_t * heap, void * payload)
{
    // ensure heap is a valid pointer
    assert(heap);

    // checks to make sure we have enough space
    ensure_space(heap);

    if (heap->data_mode == HEAP_PTR)
    {
        // adds the payload to the array
        heap->heap_array[heap->array_length] = payload;
    }
    else
    {
        uint8_t * slice = get_slice(heap, heap->array_length);
        memcpy(slice, payload, heap->node_size);
    }

    // increment the array_length of the array
    heap->array_length++;

    // perform bubble up
    bubble_up(heap);
}

/*!
 * @brief Heap sort the array passed in
 *
 * Heap sort function is used to sort the array passed in. The array can be
 * an array of pointers using the HEAP_PTR mode or an array of contiguous
 * data blocks in HEAP_MEM mode. Either way, the array passed in will be
 * sorted in place using the provided compare function
 * @param array
 * @param item_count
 * @param item_size
 * @param data_mode
 * @param type
 * @param compare
 */
void heap_sort(void * array,
               size_t item_count,
               size_t item_size,
               heap_data_mode_t data_mode,
               heap_type_t type,
               heap_compare_t (* compare)(void *, void *))
{
    assert(array);

    heap_t * heap = heap_init(type, data_mode, item_size, NULL, compare);
    if (NULL == heap)
    {
        return;
    }

    // Create the heap structure if in data mode
    if (HEAP_PTR == heap->data_mode)
    {
        for (size_t item = 0; item < item_count; item++)
        {
            heap_insert(heap, * ((void **)array + item));
        }
        for (size_t item = 0; item < item_count; item++)
        {
            * ((void **)array + item) = heap_pop(heap);
        }
    }
    else
    {
        // Insert each item into the heap then pop
        for (size_t item = 0; item < item_count; item++)
        {
            heap_insert(heap, (uint8_t *)array + get_index(item, item_size));
        }

        uint8_t * index_ptr = NULL;
        void * pop_item = NULL;
        for (size_t item = 0; item < item_count; item++)
        {
            index_ptr = (uint8_t *)array + get_index(item, item_size);
            pop_item = heap_pop(heap);
            memcpy(index_ptr, pop_item, item_size);
            free(pop_item);
        }
    }

    // set the mode to mem so that we do not free the pointers incase we are
    // in pointer mode or else the user will not have their data
    heap->data_mode = HEAP_MEM;
    heap_destroy(heap);
}


void * heap_find_nth_item(void * array,
                          size_t item_count,
                          size_t item_size,
                          size_t nth_item,
                          heap_data_mode_t data_mode,
                          heap_type_t type,
                          heap_compare_t (* compare)(void *, void *))
{
    assert(array);

    heap_t * heap = heap_init(type, data_mode, item_size, NULL, compare);
    if (NULL == heap)
    {
        return NULL;
    }

    void * target_item = NULL;

    // Create the heap structure if in data mode
    if (HEAP_PTR == heap->data_mode)
    {
        for (size_t item = 0; item < item_count; item++)
        {
            heap_insert(heap, * ((void **)array + item));
        }

        if ((nth_item < 1) || (nth_item > heap->array_length))
        {
            heap_destroy(heap);
            return NULL;
        }

        for (size_t item = 0; item < nth_item; item++)
        {
            target_item = heap_pop(heap);
        }
    }
    else
    {
        // Insert each item into the heap then pop
        for (size_t item = 0; item < item_count; item++)
        {
            heap_insert(heap, (uint8_t *)array + get_index(item, item_size));
        }

        if ((nth_item < 1) || (nth_item > heap->array_length))
        {
            heap_destroy(heap);
            return NULL;
        }

        uint8_t * index_ptr = NULL;
        void * pop_item = NULL;
        for (size_t item = 0; item < nth_item - 1; item++)
        {
            index_ptr = (uint8_t *)array + get_index(item, item_size);
            pop_item = heap_pop(heap);
            memcpy(index_ptr, pop_item, item_size);
            free(pop_item);
        }
        target_item = heap_pop(heap);
    }

    heap_destroy(heap);
    return target_item;
}

/*!
 * @brief Check to see if the data provided is already in the heap
 * @param heap
 * @param data
 * @return bool
 */
bool heap_in_heap(heap_t * heap, void * data)
{
    assert(heap);

    bool found = false;
    heap_compare_t comparison;
    size_t start_index = 0;
    while ((!found) && (start_index <= heap->array_length))
    {
        comparison = heap->compare(get_slice(heap, start_index), data);
        if (HEAP_EQ == comparison)
        {
            return true;
        }
        start_index++;
    }
    return false;
}

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
 * @brief Pop the root value of the tree. Always returns a
 * pointer that must be freed
 *
 * @param heap
 * @return Pointer that must be freed
 */
void * heap_pop(heap_t * heap)
{
    // ensure that the pointer is valid
    assert(heap);

    // check if heap is empty, if so, return
    if (heap_is_empty(heap))
    {
        return NULL;
    }

    void * payload;

     if (HEAP_PTR == heap->data_mode)
    {
        // pop the root node and decrement our array_length size
        payload = heap->heap_array[0];
        heap->array_length--;
        heap->heap_array[0] = heap->heap_array[heap->array_length];
    }
    else
    {
        // Extract the current data at index 0
        uint8_t * temp = (uint8_t *)calloc(1, sizeof(heap->node_size));
        uint8_t * index_0_ptr = get_slice(heap, 0);
        memcpy(temp, index_0_ptr, heap->node_size);

        // Place the last node at index 0 to start the bubble algorithm
        // the copied node is zeroes out
        heap->array_length--;
        uint8_t * index_last_ptr = get_slice(heap, heap->array_length);
        memcpy(index_0_ptr, index_last_ptr, heap->node_size);
        memset(index_last_ptr, 0, heap->node_size);

        // save the pointer to the variable
        payload = (void *)temp;
    }


    // perform the bubble down algorithm
    if (heap->array_length)
    {
        bubble_down(heap);
    }

    // resize array if we need to
    ensure_downgrade_size(heap);

    // return pop value
    return payload;
}

/*!
 * @brief Dynamically increase the size of the heap
 * @param heap
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
 * Change the size of the data array if the length of the array can fit
 * within the size/2 of the array. Unless, the size becomes less than the
 * base size
 * @param heap
 */
static void ensure_downgrade_size(heap_t * heap)
{
    if ((heap->array_length == (heap->array_size / 2))
        & (heap->array_size > BASE_SIZE))
    {
        heap->array_size = heap->array_size / 2;
        resize_heap(heap);
    }
}

/*!
 * Resize the array based on the what is happening dynamically.
 * @param heap
 */
static void resize_heap(heap_t * heap)
{
    void * re_alloc = NULL;
    if (HEAP_PTR == heap->data_mode)
    {
        re_alloc = realloc(heap->heap_array, sizeof(void * ) *
        heap->array_size);
    }
    else
    {
        re_alloc = realloc(heap->heap_array, heap->node_size *
        heap->array_size);

    }
    if (INVALID_PTR == verify_alloc(re_alloc))
    {
        fprintf(stderr, "[!] Could not reallocate memory for heap!\n");
        heap_destroy(heap);
        abort();
    }
    heap->heap_array = re_alloc;
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

    while ((index > 0) &&
        (heap->heap_type
            == get_comparison(heap, index, get_parent_index(index))))
    {
        swap(heap, index, get_parent_index(index));
        index = get_parent_index(index);
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
    size_t parent_index = 0;
    size_t target_index = 0;
    while ((parent_index < heap->array_length) && (!(is_valid_parent(heap, parent_index))))
    {
        // if the "target_index" or the index to swap the parent with is the
        // parent itself then we know that we are done bubbling.
        target_index = get_target_index(heap, parent_index);
        if (target_index == parent_index)
        {
            break;
        }

        // if target is identified, then swap the values
        swap(heap, parent_index, target_index);
        parent_index = target_index;
    }
}

/*!
 * @brief Gets the parent index of the index provided
 * @param index[in] index to inspect
 * @return Index of the parent
 */
static size_t get_parent_index(size_t index)
{
    return (index - 1) / 2;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static size_t get_left_child_index(size_t index)
{
    return index * 2 + 1;
}

/*!
 * @brief Gets the child index of the index provided
 * @param index[in] index to inspect
 * @return Index of the child
 */
static size_t get_right_child_index(size_t index)
{
    return index * 2 + 2;
}

/*!
 * @brief Swap the positions of the two index in the array
 * @param heap[in] heap_t
 * @param child_index[in] index of the child_index to swap from
 * @param parent_index[in] index of the new get_parent_index
 */
static void swap(heap_t * heap, size_t child_index, size_t parent_index)
{
    if (HEAP_PTR == heap->data_mode)
    {
        void * temp_payload = heap->heap_array[child_index];
        heap->heap_array[child_index] = heap->heap_array[parent_index];
        heap->heap_array[parent_index] = temp_payload;
    }
    else
    {
        // copy node to temp var
        uint8_t temp[heap->node_size];
        uint8_t * child_data = get_slice(heap, child_index);
        uint8_t * parent_data = get_slice(heap, parent_index);

        // copy the child data to temp, then replace it with the parent
        memcpy(& temp, child_data, heap->node_size);
        memcpy(child_data, parent_data, heap->node_size);

        // replace parent data with the child data
        memcpy(parent_data, & temp, heap->node_size);
    }
}

/*!
 * @brief Return bool indicating that the parent is either greater or equal
 * to its children for a max heap or if the parent is less than or equal to
 * both of its children in the case of a min heap
 * @param heap
 * @param parent_index
 * @return
 */
static bool is_valid_parent(heap_t * heap, size_t parent_index)
{
    // Heaps will only have a right child if there is a left child because of
    // the binary tree rule. Therefore, if there is no left, then just return
    // the root which means that the node is a valid parent, a lonely parent.
    if (!(has_left_child(heap, parent_index)))
    {
        return true;
    }

    // since left child exists, check if current item is greater or less than
    size_t left_child_index = get_left_child_index(parent_index);
    heap_compare_t eval = get_comparison(heap, parent_index, left_child_index);

    // if the parent is GT the left_child in a GT heap, or it is equal, then
    // the parent is valid. Otherwise, it is not. Same goes for min
    if ((eval != heap->heap_type) || (eval != HEAP_EQ))
    {
        return false;
    }

    // if there is a right child, and we get to here then do the same
    // comparison for the right child
    if (has_right_child(heap, parent_index))
    {
        size_t right_child_index = get_right_child_index(parent_index);
        eval = get_comparison(heap, parent_index, right_child_index);
        if ((eval != heap->heap_type) || (eval != HEAP_EQ))
        {
            return false;
        }
    }

    return true;
}

/*!
 * Return if the node has a left child by getting the what would be the index
 * of its child and see if that index fits in the range of items that we have
 * on the list.
 */
static bool has_left_child(heap_t * heap, size_t index)
{
    return get_left_child_index(index) < heap->array_length;
}

/*!
 * Return if the node has a right child by getting the what would be the index
 * of its child and see if that index fits in the range of items that we have
 * on the list.
 */
static bool has_right_child(heap_t * heap, size_t index)
{
    return get_right_child_index(index) < heap->array_length;
}

/*!
 * @brief return the min/max child in order to swap their oder.
 *
 * In the situation of a max heap, the largest child is returned. In the
 * situation of a min heap, the smallest child is returned. Of course, if the
 * parent is already one of those, the parent itself is returned.
 * @param heap heap_t
 * @param parent_index parent_index to inspect
 * @return Min/Max child
 */
static size_t get_target_index(heap_t * heap, size_t parent_index)
{
    // if there is no left child, then there is no right child because of the
    // rule of binary fill from left to right. Therefore, return the root.
    if (!(has_left_child(heap, parent_index)))
    {
        return parent_index;
    }

    // target index is either the max child (maxheap) or min child (min heap)
    size_t target_index = parent_index;
    heap_compare_t eval;

    // grab the left child, because we know that it is there
    size_t left_child_index = get_left_child_index(parent_index);


    // perform the comparison between the parent and the left child. The
    // return value is based on the heap type. If the evaluation between
    // parent and left is GT and the heap type is GT_type, then set the
    // parent as the target because the goal is to set the target as the
    // heaptype == eval
    eval = get_comparison(heap, parent_index, left_child_index);
    if (eval != heap->heap_type)
    {
        target_index = left_child_index;
    }


    // If there is a right child, make the comparison between the
    // target_index and the right child
    if (has_right_child(heap, parent_index))
    {
        size_t right_child_index = get_right_child_index(parent_index);
        eval = get_comparison(heap, target_index, right_child_index);
        if (eval != heap->heap_type)
        {
            target_index = right_child_index;
        }
    }
    return target_index;
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

/*!
 * @brief performs the pointer arithmetic for indexing the correct location
 * of the array.
 *
 * @param heap
 * @return Pointer the to next location that is open
 */
static uint8_t * get_slice(heap_t * heap, size_t index)
{
    return (uint8_t *)heap->heap_array + get_index(index, heap->node_size);
}

/*!
 * Small function for calculating the index. This was part of get_slice but a
 * seperation was required for the inline sort function
 * @param index
 * @param node_size
 * @return
 */
static size_t get_index(size_t index, size_t node_size)
{
    return index * node_size;
}


/*!
 * @brief Small wrapper for getting the comparisons between nodes
 * @param heap
 * @param left_index Index to "slice" in the array
 * @return The result of the comparison
 */
static heap_compare_t get_comparison(heap_t * heap,
                                     size_t left_index,
                                     size_t right_index)
{
    heap_compare_t result = 0;

    if (HEAP_PTR == heap->data_mode)
    {
        result = heap->compare(
            heap->heap_array[left_index],
            heap->heap_array[right_index]
        );
    }
    else
    {
        result = heap->compare(
            get_slice(heap, left_index),
            get_slice(heap, right_index)
        );
    }

    return result;
}

void test_func(void * data, size_t n_items, size_t n_size)
{

    for (size_t i = 0; i < n_items; i++)
    {
        void ** int_ptr = NULL;
        uint8_t val[n_size];
        memcpy(val, (uint8_t*)data + (n_size * i), n_size);
        int_ptr = (void **)val;
        printf("Int? %d\n", *(int *)int_ptr);
    }
}
