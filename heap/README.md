# Heap
This data structure uses an array to store the nodes of the user. The ndoes are access as if they were a
binary tree, therefore the indexes are not sequential. 

There are two main ways to use the data structure, by either creating a heap and interacting with the
heap structure, or by using an array you already created and heapify you array in place. The second
method will change the order of you array in place by internally creating a heap and sorting it. 

## Create a heap
To create a heap, you will need at least one external function, this function is the compare function. Since the 
data structure does not know anything about your data, you have to provide the data structure a way of comparing 
the nodes to place them in the correct location. Beyond that, you need to pick a data mode. 

### Heap data modes
The two data modes are `HEAP_PTR` and `HEAP_MEM`. The `HEAP_PTR` mode assumes that all the items in the internal
is a pointer to a block of memory you have already allocated. The `HEAP_MEM` mode assumes that the data you are
providing it should be copies to its structure. You can think of these two modes as "storing by reference (PTR)" 
or "store by value (mem)". If using `HEAP_PTR` you will need to provide a callback to a node free function. 

```c
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
```

## Heapify
The second way to use the heap is by sorting you array. You can pass your array and how to access the array. 
The library will then heapify the array to sort it in either min or max modes

```c
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
```