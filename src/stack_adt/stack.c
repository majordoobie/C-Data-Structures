#include <stack.h>
#include <stddef.h>
#include <stdlib.h>

typedef enum
{
    BASE_SIZE = 5,
} stack_default_t;

typedef struct stack_t
{
    size_t length;
    size_t size;
    void (* destroy)(stack_payload_t * payload);
    stack_payload_t ** array;
} stack_t;

stack_t * init_stack(void (* destroy)(stack_payload_t *))
{
    stack_t * stack = malloc(sizeof(* stack));
    stack->length = 0;
    stack->size = BASE_SIZE;
    stack->destroy = destroy;
    stack->array = calloc(stack->size, sizeof(stack_payload_t *));
    return stack;
}

void destroy_stack(stack_t * stack)
{
    for (size_t i = 0; i < stack->length; i++)
    {
        stack->destroy(stack->array[i]);
    }
    free(stack->array);
    free(stack);
}
