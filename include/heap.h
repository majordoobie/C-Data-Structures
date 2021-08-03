#ifndef HEAP_H
#define HEAP_H

typedef struct heap_payload_t heap_payload_t; // I Typedef their structure to something internal
typedef struct
{
    int length;
    int heap_size;
    heap_payload_t ** heap_array;
    int (* compare)(heap_payload_t * payload, heap_payload_t * payload2);
    void (* destroy)(heap_payload_t * payload);
} heap_t;

heap_t * init_heap(int (* compare)(heap_payload_t * payload, heap_payload_t * payload2), void (* destroy)(heap_payload_t * payload));
void destroy_heap(heap_t * heap);

void insert_heap(heap_t * heap, heap_payload_t * payload);

#endif //HEAP_H
