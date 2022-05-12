#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Compare functions must implement the following return types
typedef enum
{
    HEAP_GT = 1,
    HEAP_LT = -1,
    HEAP_EQ = 0
} heap_compare_t;

typedef enum
{
    MAX_HEAP,
    MIN_HEAP
} heap_type_t;

typedef struct payload_t heap_payload_t;
typedef struct heap_t heap_t;

void print_heap(heap_t * heap, void (print_test)(heap_payload_t * payload));

heap_t * init_heap(heap_compare_t (*compare)(heap_payload_t *, heap_payload_t *), void (*destroy)(heap_payload_t *), heap_type_t type);
void destroy_heap(heap_t * heap);

void insert_heap(heap_t * heap, heap_payload_t * payload);
heap_payload_t * pop_heap(heap_t * heap);

bool heap_is_empty(heap_t * heap);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //HEAP_H
