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
                       create_payload(5, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(4, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(3, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(2, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(1, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(6, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(7, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(8, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);
            bst_insert(tree,
                       create_payload(9, nullptr), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);


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
        avl_tree_t * tree;
        mock_struct_t * payload_1;
        mock_struct_t * payload_2;
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
 * Test Traversal
 */
typedef struct
{
    int index;
    int * array;
} test_traversal_t;

//TEST_F(TreeTestFixture, TraversalInOrder)
//{
//    test_traversal_t * test_struct = (test_traversal_t * )calloc(1, sizeof(test_traversal_t));
//    test_struct->array = (int *)calloc(9, sizeof(int));
//
//    for (int index = 1; index < 10; index++)
//    {
//        test_struct->array[index - 1] = index;
//    }
//
//    free(test_struct->array);
//    free(test_struct);
//}

















