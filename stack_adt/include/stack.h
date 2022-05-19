#ifndef BST_ADT_INCLUDE_STACK_H
#define BST_ADT_INCLUDE_STACK_H
#include <stdbool.h>
#include <stddef.h>

typedef struct payload_t stack_payload_t;
typedef struct stack_adt_t stack_adt_t;

stack_adt_t * stack_init(void (* destroy)(stack_payload_t *));
void stack_destroy(stack_adt_t * stack);
void stack_push(stack_adt_t * stack, stack_payload_t * payload);
stack_payload_t * stack_pop(stack_adt_t * stack);
stack_payload_t * stack_peek(stack_adt_t * stack);
stack_payload_t * stack_nth_peek(stack_adt_t * stack, size_t index);
size_t stack_size(stack_adt_t * stack);
bool stack_is_empty(stack_adt_t * stack);


#endif //BST_ADT_INCLUDE_STACK_H
