#ifndef BST_ADT_INCLUDE_STACK_H
#define BST_ADT_INCLUDE_STACK_H
#include <stdbool.h>

typedef struct payload_t stack_payload_t;
typedef struct stack_t stack_t;

stack_t * init_stack(void (* destroy)(stack_payload_t *));
void destroy_stack(stack_t * stack);
void push_stack(stack_t * stack, stack_payload_t * payload);
stack_payload_t * pop_stack(stack_t * stack);
stack_payload_t * peek_stack(stack_t * stack);
bool is_stack_empty(stack_t * stack);


#endif //BST_ADT_INCLUDE_STACK_H
