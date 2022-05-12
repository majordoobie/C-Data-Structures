#ifndef BST_ADT_INCLUDE_STACK_H
#define BST_ADT_INCLUDE_STACK_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct payload_t stack_payload_t;
typedef struct stack_adt_t stack_adt_t;

stack_adt_t * init_stack(void (* destroy)(stack_payload_t *));
void destroy_stack(stack_adt_t * stack);
void push_stack(stack_adt_t * stack, stack_payload_t * payload);
stack_payload_t * pop_stack(stack_adt_t * stack);
stack_payload_t * peek_stack(stack_adt_t * stack);
bool is_stack_empty(stack_adt_t * stack);


#ifdef __cplusplus
}
#endif //__cplusplus
#endif //BST_ADT_INCLUDE_STACK_H
