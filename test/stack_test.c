#include <check.h>
#include <stack.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct payload_t
{
    int value;
} stack_payload_t;

void stack_free(stack_payload_t * payload)
{
    free(payload);
}

stack_payload_t * create_stack_payload(int value)
{
    stack_payload_t * payload = malloc(sizeof(* payload));
    payload->value = value;
    return payload;
}

START_TEST(creation_test){
    stack_t * stack = init_stack(stack_free);
    ck_assert_ptr_ne(stack, NULL);
    destroy_stack(stack);
}END_TEST

START_TEST(stack_push_pop_test){
    stack_t * stack = init_stack(stack_free);
    // stack should be empty
    ck_assert(is_stack_empty(stack));

    push_stack(stack, create_stack_payload(2));
    push_stack(stack, create_stack_payload(2));
    push_stack(stack, create_stack_payload(2));
    push_stack(stack, create_stack_payload(5));

    // stack should not be empty
    ck_assert(!is_stack_empty(stack));

    stack_payload_t * payload = pop_stack(stack);
    ck_assert_int_eq(payload->value, 5);

    free(payload);
    destroy_stack(stack);

}END_TEST

static TFun stack_creation_test[] = {
    creation_test,
    stack_push_pop_test,
    NULL
};

static void add_tests(TCase * test_cases, TFun * test_functions)
{
    while (* test_functions) {
        // add the test from the core_tests array to the t-case
        tcase_add_test(test_cases, * test_functions);
        test_functions++;
    }
}

Suite * stack_lib_test(void)
{
    // suite creation
    Suite * bst_suite = suite_create("Stack Tests");
    TFun * test_list;
    TCase * test_cases;

    test_list = stack_creation_test;
    test_cases = tcase_create("Stack Creation Test");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    return bst_suite;
}
