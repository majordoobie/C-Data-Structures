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

typedef enum
{
    MAX_HEAP,
    MIN_HEAP
} heap_type_t;

typedef struct heap_t heap_t;

void heap_print(heap_t * heap, void (* print_test)(void * payload));

heap_t * heap_init(heap_compare_t (* compare)(void *, void *),
                   void (* destroy)(void *),
                   heap_type_t type);

void heap_destroy(heap_t * heap);
void heap_insert(heap_t * heap, void * payload);
void * heap_pop(heap_t * heap);

void * heap_peek(heap_t * heap, int index);
void heap_dump(heap_t * heap);

bool heap_is_empty(heap_t * heap);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif //HEAP_H
