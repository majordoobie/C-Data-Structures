#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>

// Compare functions must implement the following return types
typedef enum
{
    HEAP_GT,
    HEAP_LT,
    HEAP_EQ
} heap_compare_t;

typedef struct payload_t heap_payload_t;
typedef struct
{
    int length;
    int heap_size;
    heap_payload_t ** heap_array;
    heap_compare_t (* compare)(heap_payload_t * payload, heap_payload_t * payload2);
    void (* destroy)(heap_payload_t * payload);
} heap_t;

void print_heap(heap_t * heap, void (print_test)(heap_payload_t * payload));

heap_t * init_heap(heap_compare_t (* compare)(heap_payload_t * payload, heap_payload_t * payload2), void (* destroy)(heap_payload_t * payload));
void destroy_heap(heap_t * heap);

void insert_heap(heap_t * heap, heap_payload_t * payload);
heap_payload_t * pop_heap(heap_t * heap);

bool heap_is_empty(heap_t * heap);

#endif //HEAP_H
