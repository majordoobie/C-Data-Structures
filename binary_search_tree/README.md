# BST
BST is a binary tree that is self adjusting using the AVL algorithm.

The tree itself knows nothing about the node structs that you are saving. 
Everything is opaque, and it is up to the callback functions you supply to
ensure that the tree is being structured correctly. 

To create a tree you first need to define your node structure. It should ideally
have some kind of key, but it doesn't have to. Next you want to create a 
comparison and a free function.

- Comparison function will compare each node to properly create the tree structure
- Free function is called on each node when it needs to be freed

That is it! Here is an example:

```c
/*!
 * Your node structures. The struct tag DOES need to match `bst_node_payload_t`
 * but the struct name can be anything you want.
 */
typedef struct bst_node_payload_t
{
    int key;
    void * some_data;
    int more_data;
} mock_struct_t;


/*!
 * This is the comparison function that the bst_t structure will use to properly
 * place the nodes in the tree.
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
 * A function that can be called to clean up the nodes
 */
void free_payload(mock_struct_t * payload)
{
    if (NULL != payload)
    {
        free(payload->data);
        free(payload);
    }
}

avl_tree_t * tree = bst_init(compare, free_payload);
```
