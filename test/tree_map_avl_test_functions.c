#include <tree_map_avl_test_functions.h>
#include <stdio.h>

static TFun create_destroy_test_list[] = {
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

Suite * tree_map_test_suite(void)
{
    // suite creation
    Suite * bst_suite = suite_create("Tree Map Test Suite");
    TFun * test_list;
    TCase * test_cases;

    test_list = create_destroy_test_list;
    test_cases = tcase_create("Testing creation and deletion of object");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    printf("Testing treep map son\n");

    return bst_suite;
}

