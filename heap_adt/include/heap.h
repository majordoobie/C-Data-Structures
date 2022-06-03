#ifndef HEAP_H
#define HEAP_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>
#include <stddef.h>

// Compare functions must implement the following return types
typedef enum
{
    HEAP_GT = 1,
    HEAP_LT = -1,
    HEAP_EQ = 0
} heap_compare_t;

// Type of heap_adt
typedef enum
{
    MAX_HEAP,
    MIN_HEAP
} heap_type_t;

// Type def for controlling how data is saved into the cells
typedef enum
{
    HEAP_PTR,
    HEAP_MEM
} heap_data_mode_t;

// Mapping to internal structure that manages the heap_adt
typedef struct heap_t heap_t;

void heap_print(heap_t * heap, void (* print_test)(void * payload));

heap_t * heap_init(heap_type_t type,
                   heap_data_mode_t data_mode,
                   size_t payload_size,
                   void (* destroy)(void *),
                   heap_compare_t (* compare)(void *, void *));

void heap_destroy(heap_t * heap);
void heap_insert(heap_t * heap, void * payload);
void * heap_pop(heap_t * heap);

void heap_sort(void * array,
               size_t item_count,
               size_t item_size,
               heap_data_mode_t data_mode,
               heap_type_t type,
               heap_compare_t (* compare)(void *, void *));

bool heap_in_heap(heap_t * heap, void * data);

void heap_dump(heap_t * heap);

bool heap_is_empty(heap_t * heap);
void test_func(void * data, size_t n_items, size_t n_size);

void * heap_find_nth_item(void * array,
                          size_t item_count,
                          size_t item_size,
                          size_t nth_item,
                          heap_data_mode_t data_mode,
                          heap_type_t type,
                          heap_compare_t (* compare)(void *, void *));

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //HEAP_H
