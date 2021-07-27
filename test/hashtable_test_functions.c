#include <hashtable_test_functions.h>
#include <stdio.h>
#include <check.h>

START_TEST(create_structure_not_null){
    printf("Testing a okay...\n");

}END_TEST

static TFun creation_test[] = {
    create_structure_not_null,
    NULL
};

static void add_tests(TCase * test_cases, TFun * test_functions)
{
    while (* test_functions) {
        // add the test from the core_tests array to the tcase
        tcase_add_test(test_cases, * test_functions);
        test_functions++;
    }
}
Suite * hashtable_test_suite(void)
{
    // suite creation
    Suite * bst_suite = suite_create("BST Test");
    TFun * test_list;
    TCase * test_cases;

    test_list = creation_test;
    test_cases = tcase_create("Hash table creation test");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    return bst_suite;
}
