#include <heap.h>
#include <check.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct payload_t {
    int key;
    int value;
} heap_payload_t;

heap_payload_t * create_heap_payload(int key, int value)
{
    heap_payload_t * payload = malloc(sizeof(* payload));
    payload->key = key;
    payload->value = value;
    return payload;
}

int heap_compare(heap_payload_t * payload, heap_payload_t * payload2)
{
    if (payload->key > payload2->key)
    {
        return 1;
    }
    return 0;
}

void heap_destroy(heap_payload_t * payload)
{
    free(payload);
}


START_TEST(test_creation){
    heap_t * heap = init_heap(heap_compare, heap_destroy);
    ck_assert_ptr_ne(heap, NULL);
    destroy_heap(heap);
}END_TEST

START_TEST(test_creation_insert){
    heap_t * heap = init_heap(heap_compare, heap_destroy);
    ck_assert_ptr_ne(heap, NULL);

    insert_heap(heap, create_heap_payload(5, 0));
    insert_heap(heap, create_heap_payload(6, 0));
    insert_heap(heap, create_heap_payload(19, 0));
    insert_heap(heap, create_heap_payload(2, 0));
    insert_heap(heap, create_heap_payload(2, 0));
    insert_heap(heap, create_heap_payload(2, 0));

    destroy_heap(heap);
}END_TEST



static TFun heap_create_test[] = {
    test_creation,
    test_creation_insert,
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

Suite * heap_lib_test(void)
{
    // suite creation
    Suite * bst_suite = suite_create("Heap Test Suite");
    TFun * test_list;
    TCase * test_cases;

    test_list = heap_create_test;
    test_cases = tcase_create("Basic Heap Testing");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    return bst_suite;
}
