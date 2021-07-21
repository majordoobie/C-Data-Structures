#include <bst.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct node {
    struct node * parent;
    struct node * left_child;
    struct node * right_child;
    node_payload_t * key;
};

static void in_order_traversal_node(node_t * node, void (* function)(node_payload_t *));
static void pre_order_traversal_node(node_t * node, void (* function)(node_payload_t *));
static void free_node(bst_t * bst, node_t * node, bst_status_t free_payload);
static void traversal_free(bst_t * bst, node_t * node, bst_status_t free_payload);

/*!
 * @brief Initialize the tree structure to include the payload size of the node keys.
 * The bst structure also contains the compare function pointer performed on each node
 * that is passed around.
 * @param payload_size[in] The size of each node->key payload
 * @param compare[in] Pointer to a compare function that is performed on each node
 * @return Pointer to the BST structure with the bst->root set to NULL
 */
bst_t * init_bst(bst_compare_t (* compare)(node_payload_t *, node_payload_t *), void (* free_payload)(node_payload_t *))
{
    bst_t * bst = calloc(1, sizeof(* bst));
    bst->compare = compare;
    bst->free_payload = free_payload;
    return bst;
}

/*!
 * @brief Public interface for destroying the three. A private function is called in its
 * stead
 * @param bst[in] bst_t
 * @param free_payload[in] Status must include either BST_FREE_PAYLOAD_TRUE or FALSE
 */
void destroy_bst(bst_t * bst, bst_status_t free_payload)
{
    traversal_free(bst, bst->root, free_payload);
    free(bst);
}


/*!
 * @brief Create a new node with the given node_payload. Iterate over the tree using the
 * compare function supplied to the BST object and push the new node onto the tree.
 * @param bst[in] bst_t tree pointer
 * @param payload[in] node_payload_t payload used for the new node
 * @param replace[in] Option to replace or ignore equivalent nodes
 * @return bst_status_t of the operation
 */
bst_status_t insert_node(bst_t * bst, node_payload_t * payload, bst_status_t replace)
{
    // create a new_node object
    node_t * new_node = calloc(1, sizeof(* new_node));
    new_node->key = payload;

    // if root new_node is null, then add and return
    if (NULL == bst->root)
    {
        bst->root = new_node;
        return BST_INSERT_SUCCESS;
    }

    // otherwise, recurse down the tree using the compare function supplied to bst for
    // comparison
    node_t * parent_node = NULL;
    node_t * node = bst->root;
    bst_compare_t result = 0;

    // Iterate over each node while calling the specified comparison function
    while (NULL != node)
    {
        parent_node = node;
        result = bst->compare(new_node->key, node->key);

        switch (result)
        {
            case BST_LT:
                node = node->left_child;
                break;
            case BST_GT:
                node = node->right_child;
                break;
            case BST_EQ:
                if (BST_REPLACE_TRUE == replace)
                {
                    bst->free_payload(node->key);
                    node->key = new_node->key;
                    free(new_node);
                    return BST_INSERT_SUCCESS;
                }
                else if (BST_REPLACE_FALSE == replace)
                {
                    free(new_node);
                    return BST_INSERT_EQUAL;
                }
                else
                {
                    free(new_node);
                    return BST_INSERT_FAILURE;
                }
            default:
                fprintf(stderr, "Invalid bst_status_t option chosen. Please read the documentation.");
                free(new_node);
                return BST_INSERT_FAILURE;
        }
    }

    // clean up after iteration
    new_node->parent = parent_node;
    result = bst->compare(new_node->key, parent_node->key);
    if (BST_LT == result)
    {
        parent_node->left_child = new_node;
    }
    else
    {
        parent_node->right_child = new_node;
    }

    return BST_INSERT_SUCCESS;
}

/*!
 * @brief Function that calls the internal traversal function and calls the callback
 * function passed to each node to be processed.
 * @param bst[in] bst_t
 * @param type[in] Type of traversal
 * @param callback[in] Pointer to external supplied function for node processing
 */
void traversal_bst(bst_t * bst, bst_traversal_t type, void (* callback)(node_payload_t *))
{
    switch (type)
    {
        case BST_IN_ORDER:
            in_order_traversal_node(bst->root, callback);
            break;
        case BST_POST_ORDER:
            break;
        case BST_PRE_ORDER:
            pre_order_traversal_node(bst->root, callback);
            break;
        default:
            break;
    }
}

/*!
 * @brief Recursion function to travel through the tree
 * @param node[in] The current node being inspected
 * @param function[in] External function to call on each node
 */
static void in_order_traversal_node(node_t * node, void (* function)(node_payload_t *))
{
    if (NULL == node)
    {
        return;
    }
    in_order_traversal_node(node->left_child, function);
    function(node->key);
    in_order_traversal_node(node->right_child, function);
}

/*!
 * @brief Recursion function to travel through the tree
 * @param node[in] The current node being inspected
 * @param function[in] External function to call on each node
 */
static void pre_order_traversal_node(node_t * node, void (* function)(node_payload_t *))
{
    if (NULL == node)
    {
        return;
    }
    function(node->key);
    pre_order_traversal_node(node->left_child, function);
    pre_order_traversal_node(node->right_child, function);
}

/*!
 * @brief Recursion function to free each node
 * @param bst[in] bst_t
 * @param node[in] node_t
 * @param free_payload[in] bst_status_t should include either BST_FREE_PAYLOAD_FALSE or TRUE
 */
static void traversal_free(bst_t * bst, node_t * node, bst_status_t free_payload)
{
    if (NULL == node)
    {
        return;
    }
    traversal_free(bst, node->left_child, free_payload);
    traversal_free(bst, node->right_child, free_payload);
    free_node(bst, node, free_payload);
}

/*!
 * @brief Function frees the node_payload_t by calling the bst->free_payload then it frees
 * the node itself
 * @param bst[in] bst_t
 * @param node[in] node_t
 */
static void free_node(bst_t * bst, node_t * node, bst_status_t free_payload)
{

    if (BST_FREE_PAYLOAD_TRUE == free_payload)
    {
        bst->free_payload(node->key);
        free(node);
    }
    else if (BST_FREE_PAYLOAD_FALSE == free_payload)
    {
        free(node);
    }
    else
    {
        fprintf(stderr, "Invalid bst_status_t option chosen. Please read the documentation.");
    }
}
