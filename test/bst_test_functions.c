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

void print_out(my_structure * node_payload, void * ptr)
{
    printf("%d ", node_payload->value);
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

/*!
 * @brief Basic tree creationg for testing everything
 */
bst_t * create_test_30_10_28_50_29_55()
{
    // tree creation
    bst_t * tree = bst_init(compare, free_payload);

    // external structure to stuff into the tree
    bst_insert(tree, create_payload(30, 1), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(10, 2), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(28, 3), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(50, 4), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(29, 5), BST_REPLACE_FALSE);
    bst_insert(tree, create_payload(55, 5), BST_REPLACE_FALSE);

    return tree;
}
/*
 * Above this is some driver code to run the BST API
 */





/*
 * Test case section for basic creation and deletion
 */
START_TEST(tree_creation_not_null){
    bst_t * tree = bst_init(compare, free_payload);
    ck_assert_ptr_ne(tree, NULL);
    bst_destroy(tree, BST_FREE_PAYLOAD_FALSE);
}END_TEST

START_TEST(tree_creation_node_null){
    // TREE CREATION
    bst_t * tree = create_test_30_10_28_50_29_55();

    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    ck_assert_msg(tree, "Memory freed - use valgrind to get value");

}END_TEST

START_TEST(tree_creation_replace){
    // CREATE TREE
    bst_t * tree = create_test_30_10_28_50_29_55();

    my_structure * target_payload = create_payload(28, 0);
    my_structure * found_payload = bst_get_node(tree, target_payload);

    // confirm that both payloads are different entities
    ck_assert_int_eq(found_payload->value, target_payload->value);
    ck_assert_int_ne(found_payload->other_value, target_payload->other_value);
    ck_assert_ptr_ne(found_payload, target_payload);


    bst_insert(tree, target_payload, BST_REPLACE_TRUE);
    found_payload = bst_get_node(tree, target_payload);

    // Once the new struct is insert, fetch it and confirm that they are the same
    ck_assert_ptr_eq(found_payload, target_payload);


    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
}END_TEST

START_TEST(tree_node_deletion_root){
    // TEST DELETING WITH CHILDREN NODE
    bst_t * tree = create_test_30_10_28_50_29_55();
    int expected_result[5] = {50, 28, 10, 29, 55};


    // DELETE NODE
    my_structure * target_payload = create_payload(30, 0);
    bst_remove(tree, target_payload);

    // CAPTURE NDOES
    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));

    // CHECK ASSERTION
    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, expected_result[i]);
    }

    // FREE
    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    free(target_payload);
    free(iter_obj->my_struct_list);
    free(iter_obj);
}END_TEST

START_TEST(tree_node_deletion_edge){
    // TEST DELETING WITH CHILDREN NODE
    int expected_result[5] = {30, 28, 10, 50, 55};
    bst_t * tree = create_test_30_10_28_50_29_55();


    // DELETE NODE
    my_structure * target_payload = create_payload(29, 5);
    bst_remove(tree, target_payload);

    // CAPTURE NDOES
    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));


    // CHECK ASSERTION
    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, expected_result[i]);
    }

    // FREE
    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    free(target_payload);
    free(iter_obj->my_struct_list);
    free(iter_obj);

}END_TEST

START_TEST(tree_node_deletion_children){
    // TEST DELETING WITH CHILDREN NODE
    bst_t * tree = create_test_30_10_28_50_29_55();
    int expected_result[5] = {30, 29, 10, 50, 55};

    // DELETE NODE
    my_structure * target_payload = create_payload(28, 5);
    bst_remove(tree, target_payload);

    // CAPTURE NDOES
    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));


    // CHECK ASSERTION
    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, expected_result[i]);
    }

    // FREE
    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    free(target_payload);
    free(iter_obj->my_struct_list);
    free(iter_obj);

}END_TEST



////
/*
 * Test case section for basic creation and deletion
 */
////

START_TEST(traversal_order_test){
    // TREE CREATION
    bst_t * tree = create_test_30_10_28_50_29_55();

    // create a list of the order that we expected
    int values_in_order[6] = {10, 28, 29, 30, 50, 55};
    int values_pre_order[6] = {30, 28, 10, 29, 50, 55};
    int values_post_order[6] = {10, 29, 28, 55, 50, 30};

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
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_post_order[i]);
    }

    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    free(iter_obj->my_struct_list);
    free(iter_obj);
}END_TEST

START_TEST(rotation_test_insert_leaf){
    // TREE CREATION
    bst_t * tree = create_test_30_10_28_50_29_55();
    int values_pre_order_before_rotation[7] = {30, 28, 10, 5, 29, 50, 55};

    // Create iteration structure
    iter_struct_t * iter_obj = calloc(1, sizeof(* iter_obj));
    iter_obj->my_struct_list = calloc(10, sizeof(my_structure));

    // Create new payload to add
    my_structure * payload = create_payload(5, 0);
    bst_insert(tree, payload, BST_REPLACE_FALSE);


    // Compare each node to the order value we expected
    bst_traversal(tree, BST_PRE_ORDER, save_nodes, iter_obj);
    for (int i = 0; i < iter_obj->count; i++)
    {
        ck_assert_int_eq(iter_obj->my_struct_list[i]->value, values_pre_order_before_rotation[i]);
    }

    bst_destroy(tree, BST_FREE_PAYLOAD_TRUE);
    free(iter_obj->my_struct_list);
    free(iter_obj);

}END_TEST


/////////////////////////////////////////////////////////////////////////////////////////
/*
 * Enable or Disable tests from each group
 */
static TFun rotation_tests[] = {
//    rotation_test_insert_leaf,
    NULL
};

static TFun traversal_bst_test[] = {
//    traversal_order_test,
    NULL
};

static TFun create_destroy_test_list[] = {
//    tree_creation_not_null,
//    tree_creation_node_null,
//    tree_creation_replace,
    tree_node_deletion_children,
    tree_node_deletion_edge,
    tree_node_deletion_root,
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
    TFun * test_list;
    TCase * test_cases;

    test_list = create_destroy_test_list;
    test_cases = tcase_create("Tree Basic Creation and Deletion");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    test_list = traversal_bst_test;
    test_cases = tcase_create("Traversal Test Functions");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);

    test_list = rotation_tests;
    test_cases = tcase_create("Rotation Test Functions");
    add_tests(test_cases, test_list);
    suite_add_tcase(bst_suite, test_cases);



    return bst_suite;
}
