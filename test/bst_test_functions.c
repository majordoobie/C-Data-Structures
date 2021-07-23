#include "bst_test_functions.h"

bst_compare_t compare(my_structure * current_payload, my_structure * new_payload)
{
    if (new_payload->value < current_payload->value)
    {
        return BST_LT;
    }
    else if (new_payload->value > current_payload->value)
    {
        return BST_GT;
    }
    else
    {
        return BST_EQ;
    }
}

void print(my_structure * node_payload)
{
    printf("%d", node_payload->value);
}

// Save nodes into a list
void save_nodes(my_structure * node_payload, void * ptr)
{
    iter_struct_t * iter = (iter_struct_t *) ptr;
    iter->my_struct_list[iter->count] = node_payload;
    iter->count++;
}

void free_payload(my_structure * node_payload)
{
    free(node_payload);
}


my_structure * create_payload(int val1, int val2)
{
    my_structure * data = calloc(1, sizeof(* data));
    data->value = val1;
    data->other_value = val2;
    return data;
}

/*
 * Above this is some driver code to run the BST API
 */

//START_TEST(bst_create_destroy)
//{
//    my_structure * p1 = create_payload(30, 1);
//    my_structure * p2 = create_payload(10, 2);
//    my_structure * p3 = create_payload(28, 3);
//    my_structure * p4 = create_payload(50, 4);
//    my_structure * p5 = create_payload(29, 5);
//    my_structure * p6 = create_payload(55, 5);
//    my_structure * payloads[6] = {p1, p2, p3, p4, p5, p6};
//    int values_in_order[6] = {10, 28, 29, 30, 50, 55};
//    int values_pre_order[6] = {30, 10, 28, 29, 50, 55};
//    int values_post_order[6] = {29, 28, 10, 55, 50, 30};
//
//    bst_t * tree = bst_init(compare, free_payload);
//
//    // Make sure that we are not getting null from pointer
//    ck_assert_ptr_ne(NULL, tree);
//
//    // Add values then convert to an array to test their positions
//    for (int i = 0; i < 6; i++)
//    {
//        bst_insert(tree, payloads[i], BST_REPLACE_FALSE);
//    }
//
//    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
//    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));
//
//
//    // Compare each node to the order value we expected
//    bst_traversal(tree, BST_IN_ORDER, save_nodes, iter_obj);
//    for (int i = 0; i < iter_obj->count; i++)
//    {
//        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_in_order[i]);
//    }
//
//    iter_obj->count = 0;
//    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
//    for (int i = 0; i < iter_obj->count; i++)
//    {
//        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_pre_order[i]);
//    }
//
//    iter_obj->count = 0;
//    bst_traversal(tree, BST_POST_ORDER, save_nodes, iter_obj);
//    print_2d(tree, print);
//    for (int i = 0; i < iter_obj->count; i++)
//    {
//        printf("[%d] [ %d ] == [ %d ]\n", i, iter_obj->my_struct_list[i]->value, values_post_order[i]);
//        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_post_order[i]);
//    }
//
//
//} END_TEST

START_TEST(tree_creation_not_null){
    bst_t * tree = bst_init(compare, free_payload);
    ck_assert_ptr_ne(tree, NULL);
    bst_destroy(tree, BST_FREE_PAYLOAD_FALSE);
}END_TEST

START_TEST(tree_creation_node_null){
    // tree creation
    bst_t * tree = bst_init(compare, free_payload);

    // external structure to stuff into the tree
    bst_insert(tree, create_payload(30, 1), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(10, 2), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(28, 3), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(50, 4), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(29, 5), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(55, 5), BST_REPLACE_FALSE);

    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    ck_assert_msg(tree, "Memory freed - use valgrind to get value");

}END_TEST

START_TEST(tree_creation_replace){
    // tree creation
    bst_t * tree = bst_init(compare, free_payload);

    // external structure to stuff into the tree
    bst_insert(tree, create_payload(30, 1), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(10, 2), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(28, 3), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(50, 4), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(29, 5), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(55, 5), BST_REPLACE_FALSE);

    my_structure * target_payload = create_payload(28, 5);
    my_structure * found_payload = bst_get_node(tree, target_payload);

    // confirm that both payloads are different entities
    ck_assert_int_eq(found_payload->value, target_payload->value);
    ck_assert_int_ne(found_payload->other_value, target_payload->other_value);
    ck_assert_ptr_ne(found_payload, target_payload);


    bst_insert(tree, target_payload, BST_REPLACE_TRUE);
    found_payload = bst_get_node(tree, target_payload);
    ck_assert_ptr_eq(found_payload, target_payload);


    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);



}END_TEST


static TFun create_destroy_test_list[] = {
    tree_creation_not_null,
    tree_creation_node_null,
    tree_creation_replace,
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

Suite * bst_test_suite(void)
{
    // suite creation
    Suite * bst_suite = suite_create("BST Test");

    TFun * test_list = create_destroy_test_list;
    TCase * test_cases = tcase_create("Tree Basic Creation and Deletion");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    return bst_suite;



//    Suite *s;
//    TCase *tc_core;
//
//    s = suite_create("BST Test");
//
//    tc_core = tcase_create("Core");
//
//    tcase_add_test(tc_core, bst_create_destroy);
//    suite_add_tcase(s, tc_core);

//    return s;
}
