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

// Type of heap
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

// Mapping to internal structure that manages the heap
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

void heap_sort(void ** array, size_t item_count, heap_compare_t (*
compare)(void *, void *), heap_type_t type);

void * heap_peek(heap_t * heap, int index);
void heap_dump(heap_t * heap);

bool heap_is_empty(heap_t * heap);
void fuck_with_it();

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //HEAP_H
