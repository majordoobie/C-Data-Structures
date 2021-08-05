#include <stack.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
    BASE_SIZE = 5,
} stack_default_t;

static void ensure_space(stack_t * stack);
static void ensure_downgrade_space(stack_t * stack);
static void resize_stack(stack_t * stack);

typedef struct stack_t
{
    size_t length;
    size_t size;
    void (* destroy)(stack_payload_t * payload);
    stack_payload_t ** array;
} stack_t;

/*!
 * @brief Initialize a stack
 * @param destroy[in] Callback function to a destroy function for the provided payloads
 * @return Stack pointer
 */
stack_t * init_stack(void (* destroy)(stack_payload_t *))
{
    stack_t * stack = malloc(sizeof(* stack));
    stack->length = 0;
    stack->size = BASE_SIZE;
    stack->destroy = destroy;
    stack->array = calloc(stack->size, sizeof(stack_payload_t *));
    return stack;
}

/*!
 * @brief Iterates over the stack and calls the destroy callback on each payload then
 * frees the stack itself
 * @param stack[in] stack_t
 */
void destroy_stack(stack_t * stack)
{
    for (size_t i = 0; i < stack->length; i++)
    {
        stack->destroy(stack->array[i]);
    }
    free(stack->array);
    free(stack);
}

/*!
 * @brief Push a payload onto the stack
 * @param stack[in] stack_t
 * @param payload[in] stack_payload_t
 */
void push_stack(stack_t * stack, stack_payload_t * payload)
{
    // Ensure that there is enough space in the stack to add another payload
    ensure_space(stack);
    stack->array[stack->length] = payload;
    stack->length++;
}

/*!
 * @brief Pop a payload from the stack
 * @param stack[in] stack_t
 * @return Pointer to the payload that was removed
 */
stack_payload_t * pop_stack(stack_t * stack)
{
    if (is_stack_empty(stack))
    {
        return NULL;
    }
    stack->length--;
    stack_payload_t * payload = stack->array[stack->length];
    stack->array[stack->length] = 0;
    ensure_downgrade_space(stack);
    return payload;
}

/*!
 * @brief Returns the top most payload without removing it from the stack
 * @param stack[in] stack_t
 * @return Pointer to the payload on the stack
 */
stack_payload_t * peek_stack(stack_t * stack)
{
    if (is_stack_empty(stack))
    {
        return NULL;
    }
    stack_payload_t * payload = stack->array[stack->length - 1];
    return payload;
}

bool is_stack_empty(stack_t * stack)
{
    return stack->length == 0;
}

static void ensure_space(stack_t * stack)
{
    if (stack->length == stack->size)
    {
        stack->size = stack->size * 2;
        resize_stack(stack);
    }
}
static void ensure_downgrade_space(stack_t * stack)
{
    if (stack->length == (stack->size) / 2)
    {
        stack->size = stack->size / 2;
        resize_stack(stack);
    }
}

static void resize_stack(stack_t * stack)
{
    stack_payload_t ** re_alloc = realloc(stack->array, sizeof(stack->array) * stack->size);
    if (NULL == re_alloc)
    {
        fprintf(stderr, "Could not reallocate memory for stack!");
        abort();
    }
    stack->array = re_alloc;
}