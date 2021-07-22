#include <stdio.h>
#include <stdlib.h>
#include <bst.h>
#include <check.h>
#include <math.h>
#include <bst_test_functions.h>


START_TEST(bst_create_destroy)
{
    my_structure * p1 = create_payload(30, 1);
    my_structure * p2 = create_payload(10, 2);
    my_structure * p3 = create_payload(28, 3);
    my_structure * p4 = create_payload(50, 4);
    my_structure * p5 = create_payload(29, 5);
    my_structure * p6 = create_payload(55, 5);
    my_structure * payloads[6] = {p1, p2, p3, p4, p5, p6};
    int values_in_order[6] = {10, 28, 29, 30, 50, 55};
    int values_pre_order[6] = {30, 10, 28, 29, 50, 55};
    int values_post_order[6] = {29, 28, 10, 55, 50, 30};

    bst_t * tree = bst_init(compare, free_payload);

    // Make sure that we are not getting null from pointer
    ck_assert_ptr_ne(NULL, tree);

    // Add values then convert to an array to test their positions
    for (int i = 0; i < 6; i++)
    {
        bst_insert(tree, payloads[i], BST_REPLACE_FALSE);
    }

    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));


    // Compare each node to the order value we expected
    bst_traversal(tree, BST_IN_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_in_order[i]);
    }

    iter_obj->count = 0;
    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_pre_order[i]);
    }

    iter_obj->count = 0;
    bst_traversal(tree, BST_POST_ORDER, save_nodes, iter_obj);
    print_2d(tree, print);
    for (int i = 0; i < iter_obj->count; i++)
    {
        printf("[%d] [ %d ] == [ %d ]\n", i, iter_obj->my_struct_list[i]->value, values_post_order[i]);
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_post_order[i]);
    }
} END_TEST

Suite * test_poly_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("BST Test");

    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, bst_create_destroy);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_poly_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
