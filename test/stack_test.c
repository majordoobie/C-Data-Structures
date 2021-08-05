#include <check.h>
#include <stack.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct stack_payload_t
{
    int value;
} payload_t;

void stack_free(stack_payload_t * payload)
{
    free(payload);
}

payload_t * create_stack_payload(int value)
{
    payload_t * payload = malloc(sizeof(* payload));
    payload->value = value;
    return payload;
}

START_TEST(creation_test){
    stack_t * stack = init_stack(stack_free);
    ck_assert_ptr_ne(stack, NULL);
    destroy_stack(stack);
}END_TEST

static TFun stack_creation_test[] = {
    creation_test,
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
