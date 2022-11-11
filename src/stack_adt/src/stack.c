#include <stack.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  BASE_SIZE = 5,
} stack_default_t;

static void ensure_space(stack_adt_t *stack);
static void ensure_downgrade_space(stack_adt_t *stack);
static void resize_stack(stack_adt_t *stack);

typedef struct stack_adt_t {
  size_t length;
  size_t size;
  void (*destroy)(stack_payload_t *payload);
  stack_payload_t **array;
} stack_adt_t;

/*!
 * @brief Initialize a stack
 * @param destroy[in] Callback function to a destroy function for the provided
 * payloads
 * @return Stack pointer
 */
stack_adt_t *stack_init(void (*destroy)(stack_payload_t *)) {
  stack_adt_t *stack = malloc(sizeof(*stack));
  stack->length = 0;
  stack->size = BASE_SIZE;
  stack->destroy = destroy;
  stack->array = calloc(stack->size, sizeof(stack_payload_t *));
  return stack;
}

/*!
 * @brief Iterates over the stack and calls the destroy callback on each payload
 * then frees the stack itself
 * @param stack[in] stack_adt_t
 */
void stack_destroy(stack_adt_t *stack) {
  for (size_t i = 0; i < stack->length; i++) {
    stack->destroy(stack->array[i]);
  }
  free(stack->array);
  free(stack);
}

/*!
 * @brief Push a payload onto the stack
 * @param stack[in] stack_adt_t
 * @param payload[in] stack_payload_t
 */
void stack_push(stack_adt_t *stack, stack_payload_t *payload) {
  // Ensure that there is enough space in the stack to add another payload
  ensure_space(stack);
  stack->array[stack->length] = payload;
  stack->length++;
}

/*!
 * @brief Pop a payload from the stack
 * @param stack[in] stack_adt_t
 * @return Pointer to the payload that was removed
 */
stack_payload_t *stack_pop(stack_adt_t *stack) {
  if (stack_is_empty(stack)) {
    return NULL;
  }
  stack->length--;
  stack_payload_t *payload = stack->array[stack->length];
  stack->array[stack->length] = 0;
  ensure_downgrade_space(stack);
  return payload;
}

/*!
 * @brief Returns the top most payload without removing it from the stack
 * @param stack[in] stack_adt_t
 * @return Pointer to the payload on the stack
 */
stack_payload_t *stack_peek(stack_adt_t *stack) {
  return stack_nth_peek(stack, stack->length - 1);
}

/*!
 * @brief Returns the payload at the nth position on the stack. The
 * fuction does NOT pop items off the stack, so no free is necessary.
 *
 * @param stack[in] stack_adt_t
 * @param index element to inspect
 * @return Pointer to the payload on the stack
 */
stack_payload_t *stack_nth_peek(stack_adt_t *stack, size_t index) {
  if (stack_is_empty(stack)) {
    return NULL;
  }

  if (index < stack->length) {
    return stack->array[index];
  }
  return NULL;
}

/*!
 * @brief Dumps and frees all elements in the stack
 * @param stack
 */
void stack_dump(stack_adt_t *stack) {
  stack_payload_t *payload;
  while (!stack_is_empty(stack)) {
    payload = stack_pop(stack);
    stack->destroy(payload);
  }
}

size_t stack_size(stack_adt_t *stack) { return stack->length; }

bool stack_is_empty(stack_adt_t *stack) { return stack->length == 0; }

static void ensure_space(stack_adt_t *stack) {
  if (stack->length == stack->size) {
    stack->size = stack->size * 2;
    resize_stack(stack);
  }
}

static void ensure_downgrade_space(stack_adt_t *stack) {
  if ((stack->length == (stack->size) / 2) & (stack->length > BASE_SIZE)) {
    stack->size = stack->size / 2;
    resize_stack(stack);
  }
}

static void resize_stack(stack_adt_t *stack) {
  stack_payload_t **re_alloc =
      realloc(stack->array, sizeof(stack->array) * stack->size);
  if (NULL == re_alloc) {
    fprintf(stderr, "Could not reallocate memory for stack!");
    abort();
  }
  stack->array = re_alloc;
}
