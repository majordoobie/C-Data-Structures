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
    free(payload->data);
    free(payload);
}

/*!
 * @brief helper function to create payloads. Not part of bst
 */
mock_struct_t * create_payload(int val)
{
    mock_struct_t * payload = (mock_struct_t *)calloc(1, sizeof(mock_struct_t));
    payload->key = val;
    payload->data = (void *)strdup("Some data string");
    return payload;
}

// This test is just to ensure we can compile. Nothing fancy happening
TEST(INITIAL_TEST, INIT_TEST_TO_COMPILE)
{
    bst_t * tree = bst_init(compare, free_payload);
    EXPECT_NE(tree, nullptr);
    bst_insert(tree, create_payload(1), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);

    // Create another payload with the same key to remove it
    mock_struct_t * payload = create_payload(1);
    mock_struct_t * fetched_payload = bst_get_node(tree, payload);

    // ensure that they are the same keys
    EXPECT_EQ(fetched_payload->key, payload->key);

    // clean up
    bst_destroy(tree, FREE_PAYLOAD_TRUE);


    free_payload(payload);
}


class TreeTestFixture : public ::testing::Test
{
    protected:
        void SetUp() override
        {
            // expected order it 5->4->3->2->1->6->7->8->9->6
            bst_insert(tree, create_payload(5), REPLACE_PAYLOAD_FALSE,nullptr, nullptr);
            bst_insert(tree, create_payload(4), REPLACE_PAYLOAD_FALSE,nullptr, nullptr);
            bst_insert(tree, create_payload(3), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(2), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(1), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(6), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(7), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(8), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
            bst_insert(tree, create_payload(9), REPLACE_PAYLOAD_FALSE,   nullptr,nullptr);
        }
        void TearDown() override
        {
            bst_destroy(tree, FREE_PAYLOAD_TRUE);
        }

    public:
        bst_t * tree = bst_init(compare, free_payload);



};

TEST_F(TreeTestFixture, Sample)
{
    bst_insert(tree, create_payload(1), REPLACE_PAYLOAD_FALSE, nullptr, nullptr);

    // Create another payload with the same key to remove it
    mock_struct_t * payload = create_payload(1);
    mock_struct_t * fetched_payload = bst_get_node(tree, payload);

    // ensure that they are the same keys
    EXPECT_EQ(fetched_payload->key, payload->key);

}



/*
 * -> Changing sizes of mains
 * H + k : expand main pane
 * H + j : shrink main pane
 *
 * -> Move focus
 * H + l : Move focus clockwise
 * H + h : Move focus counter clockwise
 * H + ; : Move focus to clockwise screen
 *
 * -> Moving windows
 * H + o : swap focused window clockwise
 * H + i : swap focused window counter clockwise
 * H + m : swap focused window to main window
 * H + ' : swap focus window to clockwise screen
 * throw windows to space ${int}
 *
 * -> Layouts
 * H + f : select fullscreen
 * H + n : cycle layout forward
 *
 */






