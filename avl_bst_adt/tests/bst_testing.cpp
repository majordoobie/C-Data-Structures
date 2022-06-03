#include <gtest/gtest.h>

extern "C"
{
     #include <bst.h>
}

/*
 * bst_node_payload_t is the INTERNAL struct tag name. You must typedef the
 * expected struct tag with whatever alias you would like to use.
 */
typedef struct bst_node_payload_t
{
    int key;
    void * data;
} mock_struct_t;


/*!
 * @brief BST API requires a supplied comparison function to be called on the
 * supplied structure
 */
bst_compare_t compare(mock_struct_t * current_payload, mock_struct_t * new_payload)
{
    if (new_payload->key < current_payload->key)
    {
        return BST_LT;
    }
    else if (new_payload->key > current_payload->key)
    {
        return BST_GT;
    }
    else
    {
        return BST_EQ;
    }
}
/*!
 * @brief In addition to a compare function, bst requires a supplied
 * free_payload function.
 */
void free_payload(mock_struct_t * payload)
{
    if (NULL != payload)
    {
        free(payload->data);
        free(payload);
    }
}

/*!
 * @brief helper function to create payloads. Not part of bst
 */
mock_struct_t *create_payload(int val, const char *string)
{
    mock_struct_t * payload = (mock_struct_t *)calloc(1, sizeof(mock_struct_t));
    payload->key = val;

    if (nullptr == string)
    {
        payload->data = (void *)strdup("Some data string");
    }
    else
    {
        payload->data = (void *)strdup(string);
    }
    return payload;
}

class TreeTestFixture : public ::testing::Test
{
 protected:
    void SetUp() override
    {
        // expected order it 5->4->3->2->1->6->7->8->9->6
        tree = bst_init(compare, free_payload);
        bst_insert(tree,
                   create_payload(5, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(4, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(3, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(2, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(1, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(6, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(7, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(8, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);
        bst_insert(tree,
                   create_payload(9, nullptr),
                   REPLACE_PAYLOAD_FALSE,
                   nullptr,
                   nullptr);


        // create single nodes for testing
        payload_1 = create_payload(1, nullptr);
        payload_2 = create_payload(1, "New String");
    }
    void TearDown() override
    {
        bst_destroy(tree, FREE_PAYLOAD_TRUE);
        free_payload(payload_1);
        free_payload(payload_2);
    }

 public:
    avl_tree_t *tree;
    mock_struct_t *payload_1;
    mock_struct_t *payload_2;


    /*!
     * Define the struct type used for testing the traversal orders
     * This method is used as a callback for the traversal functions. This is
     * for the purpose of testing that the correct order if found. The function
     * will save each index into the saved memory space and later on it is used
     * to test the the correct order was extracted from testing
     * @param payload
     * @param void_array
     * @return
     *
     */
    typedef struct
    {
        int index;
        int * array;
    } test_traversal_t;

    /*!
     * @brief This is a brief
     * @param payload
     * @param void_array
     * @return My return type
     */
    static bst_recurse_t test_order(mock_struct_t *payload, void *void_array)
    {
        test_traversal_t *array = (test_traversal_t *)void_array;

        array->array[array->index] = payload->key;
        array->index++;

        return RECURSE_TRUE;
    }
};

// Test that we can fetch a payload that exists
TEST_F(TreeTestFixture, TestFetchingOfPayload)
{
    // Fetch the payload with the same key
    mock_struct_t * fetched_payload = bst_get_node(tree, payload_1);

    // ensure that they are the same keys
    EXPECT_EQ(fetched_payload->key, payload_1->key);
}

// Test the replacement of a payload
TEST_F(TreeTestFixture, TestPayloadReplacement)
{
    // Fetch the payload with the same key
    mock_struct_t * fetched_payload = bst_get_node(tree, payload_1);


    // The default string is "Some data string"
    EXPECT_EQ(0, strcmp((char*)fetched_payload->data, "Some data string"));

    // We are going to try to insert the new payload value but setting
    // replace to false and then to true
    bst_insert(tree, payload_2, REPLACE_PAYLOAD_FALSE, nullptr, nullptr);

    // Fetch the payload with the same key
    fetched_payload = bst_get_node(tree, payload_1);
    EXPECT_EQ(0, strcmp((char*)fetched_payload->data, "Some data string"));

    // Do it again, this time with the enabled flag
    bst_insert(tree, payload_2, REPLACE_PAYLOAD_TRUE, nullptr, nullptr);

    // make sure to set var to null so that we do not clean up twice
    // this is just an issue with mock testing
    payload_2 = nullptr;

    // Fetch the payload with the same key
    fetched_payload = bst_get_node(tree, payload_1);
    EXPECT_EQ(0, strcmp((char*)fetched_payload->data, "New String"));
}


/*
 * Test that the order saved into the test_traversal_t is the order that the
 * traversal test is expecting
 */
TEST_F(TreeTestFixture, TraversalInOrder)
{
    int array_count = 9;
    int expected_array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    test_traversal_t * test_struct = (test_traversal_t * )calloc(1, sizeof(test_traversal_t));
    test_struct->array = (int *)calloc(array_count, sizeof(int));

    bst_traversal(tree, TRAVERSAL_IN_ORDER, test_order, test_struct);

    for (int index = 0; index < array_count; index++)
    {
        EXPECT_EQ(expected_array[index], test_struct->array[index]);
    }

    free(test_struct->array);
    free(test_struct);
}

TEST_F(TreeTestFixture, TraversalPreOrder)
{
    int array_count = 9;
    int expected_array[] = {4, 2, 1, 3, 6, 5, 8, 7, 9};

    test_traversal_t * test_struct = (test_traversal_t * )calloc(1, sizeof(test_traversal_t));
    test_struct->array = (int *)calloc(array_count, sizeof(int));

    bst_traversal(tree, TRAVERSAL_PRE_ORDER, test_order, test_struct);

    for (int index = 0; index < array_count; index++)
    {
        EXPECT_EQ(expected_array[index], test_struct->array[index]);
    }

    free(test_struct->array);
    free(test_struct);
}

TEST_F(TreeTestFixture, TraversalPostOrder)
{
    int array_count = 9;
    int expected_array[] = {1, 3, 2, 5, 7, 9, 8, 6, 4};

    test_traversal_t * test_struct = (test_traversal_t * )calloc(1, sizeof(test_traversal_t));
    test_struct->array = (int *)calloc(array_count, sizeof(int));

    bst_traversal(tree, TRAVERSAL_POST_ORDER, test_order, test_struct);

    for (int index = 0; index < array_count; index++)
    {
        EXPECT_EQ(expected_array[index], test_struct->array[index]);
    }

    free(test_struct->array);
    free(test_struct);
}

// Test ability to remove a node from the tree
TEST_F(TreeTestFixture, TestNodeDeletion)
{
    bst_node_payload_t * payload = create_payload(5, nullptr);
    bst_status_t  status = bst_remove(this->tree, payload, FREE_PAYLOAD_TRUE);
    EXPECT_EQ(status, BST_REMOVE_SUCCESS);
    free_payload(payload);
}













