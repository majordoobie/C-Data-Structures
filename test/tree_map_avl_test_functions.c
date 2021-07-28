#include <tree_map_avl_test_functions.h>
#include <stdio.h>
#include <check.h>
#include <bst.h>
#include <stdlib.h>
#include <tree_map_avl.h>

typedef struct bst_node_payload_t
{
    char ** student_name;
    double grade_avg;
} key_val_t;

bst_compare_t map_compare(key_val_t * current_payload, key_val_t * new_payload)
{
    return BST_EQ;
}

void map_free_payload(key_val_t * node_payload)
{
    free(node_payload->student_name);
    free(node_payload);
}

key_val_t * create_key_val_t(const char * name, double grade_avg)
{
    key_val_t * student = calloc(1, sizeof(* student));
    return student;
}

dict_t * create_dict()
{
    return init_dict(map_compare, map_free_payload);
}
// TEST CODE BELOW

START_TEST(create_destroy_test){
    dict_t * dict = create_dict();
    ck_assert_ptr_ne(dict, NULL);
    destroy_dict(dict);

}END_TEST

static TFun create_destroy_test_list[] = {
    create_destroy_test,
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

