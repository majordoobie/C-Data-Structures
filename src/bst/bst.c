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

static void free_node(bst_t * bst, node_t * node, bst_status_t free_payload);
static void traversal_free(bst_t * bst, node_t * node, bst_status_t free_payload);
static void print_2d_iter(node_t * node, int space, void (* callback)(node_payload_t *));
static void in_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr);
static void pre_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr);
static void post_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr);
static void right_rotation(bst_t * bst, node_t * node);
static void left_rotation(bst_t *bst, node_t *node);
static node_t * search_node(bst_t * bst, node_t * node, node_payload_t * target_payload);

void
left_rotation(bst_t *bst, node_t *node);
void print_2d(bst_t * bst, void (*callback)(node_payload_t *))
{
    print_2d_iter(bst->root, 0, callback);
}

/*!
 * @brief Initialize the tree structure to include the payload size of the node keys.
 * The bst structure also contains the compare function pointer performed on each node
 * that is passed around.
 * @param payload_size[in] The size of each node->key payload
 * @param compare[in] Pointer to a compare function that is performed on each node
 * @return Pointer to the BST structure with the bst->root set to NULL
 */
bst_t * bst_init(bst_compare_t (* compare)(node_payload_t *, node_payload_t *), void (* free_payload)(node_payload_t *))
{
    bst_t * bst = calloc(1, sizeof(* bst));
    bst->compare = compare;
    bst->free_payload = free_payload;
    return bst;
}

/*!
 * @brief Public interface for destroying the tree. A private function is called in its
 * stead
 * @param bst[in] bst_t
 * @param free_payload[in] Status must include either BST_FREE_PAYLOAD_TRUE or FALSE
 */
void bst_destroy(bst_t * bst, bst_status_t free_payload)
{
    traversal_free(bst, bst->root, free_payload);
    free(bst);
    bst = NULL;
}


/*!
 * @brief Create a new node with the given node_payload. Iterate over the tree using the
 * compare function supplied to the BST object and push the new node onto the tree.
 * @param bst[in] bst_t tree pointer
 * @param payload[in] node_payload_t payload used for the new node
 * @param replace[in] Option to replace or ignore equivalent nodes
 * @return bst_status_t of the operation
 */
bst_status_t bst_insert(bst_t * bst, node_payload_t * payload, bst_status_t replace)
{
    if (NULL == payload)
    {
        return BST_INSERT_FAILURE;
    }

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
        result = bst->compare(node->key, new_node->key);

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
                    // we no longer need this node
                    free(new_node);
                    bst->free_payload(node->key);
                    node->key = payload;
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
    result = bst->compare(parent_node->key, new_node->key);
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
 * @brief Public function to find a given payload by utilizing the registered compare
 * callback function. For this reason, primitives can not be used as a search term. A
 * whole node_payload_t has to be created for the purpose of finding the right node.
 * @param bst[in] bst_t
 * @param payload[in] node_payload_t this is a "temp" payload to utilize the key for the
 * compare function
 * @return Returns the pointer to the note_payload_t if found, otherwise a NULL is returned
 */
node_payload_t * bst_get_node(bst_t * bst, node_payload_t * payload)
{
    node_t * node = search_node(bst, bst->root, payload);
    return node->key;
}

/*!
 * @brief This public function serves as a "test" for proper rotation. Rotations are normally
 * executed automatically by the tree inserts
 * @param bst[in] bst_t
 * @param payload[in] node_payload_t
 * @param side[in] BST_ROTATE_LEFT BST_ROTATE_RIGHT
 * @return bst_status_t indicating if rotation was a success
 */
bst_status_t rotate(bst_t * bst, node_payload_t * payload, bst_status_t side)
{
    node_t * node = search_node(bst, bst->root, payload);

    if (NULL == node)
    {
        return BST_SEARCH_FAILURE;
    }

    if (BST_ROTATE_RIGHT == side)
    {
        right_rotation(bst, node);
    }
    else if (BST_ROTATE_LEFT == side)
    {
        left_rotation(bst, node);
    }
    else
    {
        return BST_ROTATE_FAILURE;
    }

    return BST_ROTATION_SUCCESS;

}


/*!
 * @brief Function that calls the internal traversal function and calls the callback
 * function passed to each node to be processed.
 * @param bst[in] bst_t
 * @param type[in] Type of traversal
 * @param callback[in] Pointer to external supplied function for node processing
 */
void bst_traversal(bst_t * bst, bst_traversal_t type, void (* callback)(node_payload_t *, void *), void * void_ptr)
{
    switch (type)
    {
        case BST_IN_ORDER:
            in_order_traversal_node(bst->root, callback, void_ptr);
            break;
        case BST_POST_ORDER:
            post_order_traversal_node(bst->root, callback, void_ptr);
            break;
        case BST_PRE_ORDER:
            pre_order_traversal_node(bst->root, callback, void_ptr);
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
static void in_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return;
    }
    in_order_traversal_node(node->left_child, function, void_ptr);
    function(node->key, void_ptr);
    in_order_traversal_node(node->right_child, function, void_ptr);
}

/*!
 * @brief Recursion function to travel through the tree
 * @param node[in] The current node being inspected
 * @param function[in] External function to call on each node
 */
static void pre_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return;
    }
    function(node->key, void_ptr);
    pre_order_traversal_node(node->left_child, function, void_ptr);
    pre_order_traversal_node(node->right_child, function, void_ptr);
}

static void post_order_traversal_node(node_t * node, void (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return;
    }
    post_order_traversal_node(node->left_child, function, void_ptr);
    post_order_traversal_node(node->right_child, function, void_ptr);
    function(node->key, void_ptr);
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
/*!
 * @brief Function to recursively printout the nodes in a manner that you can visually see
 * in the terminal
 * @param node[in] node_t
 * @param space[in] Value of how many spaces to print
 * @param callback[in] A callback function to print out the data
 */
static void print_2d_iter(node_t * node, int space, void(*callback)(node_payload_t*))
{
    if (NULL == node)
    {
        return;
    }

    space += 10;
    print_2d_iter(node->right_child, space, callback);

    printf("\n");
    for (int i = 10; i < space; i++)
    {
        printf(" ");
    }
    callback(node->key);

    print_2d_iter(node->left_child, space, callback);
}

static void right_rotation(bst_t * bst, node_t * node)
{
    if ((NULL == node) || (bst->root == node))
    {
        return;
    }

    // set the constants
    node_t * old_parent = node->parent;
    node_t * old_p_parent = old_parent->parent;
    node_t * old_right = node->right_child;

    // start node rotation
    node->parent = old_p_parent;
    node->right_child = old_parent;

    // update the old_parent
    old_parent->parent = node;
    old_parent->left_child = old_right;

    // update old_right
    if (NULL != old_right)
    {
        old_right->parent = old_parent;
    }

    // update old_p_parent if it exists, it only doesn't when root is the old_p_p
    if (NULL != old_p_parent)
    {
        old_p_parent->left_child = node;
    }
    else
    {
        // if old_p_p IS NULL then we need to set the new root
        bst->root = node;
    }

}

static void left_rotation(bst_t * bst, node_t * node)
{
    if ((NULL == node) || (bst->root == node))
    {
        return;
    }

    // set the constants
    node_t * old_parent = node->parent;
    node_t * old_p_parent = old_parent->parent;
    node_t * old_left = node->left_child;

    // start node rotation
    node->parent = old_p_parent;
    node->left_child = old_parent;

    // update the old_parent
    old_parent->parent = node;
    old_parent->right_child = old_left;

    // update old_left
    if (NULL != old_left)
    {
        old_left->parent = old_parent;
    }

    // update old_p_parent if it exists, it only doesn't when root is the old_p_p
    if (NULL != old_p_parent)
    {
        old_p_parent->right_child = node;
    }
    else
    {
        // if old_p_p IS NULL then we need to set the new root
        bst->root = node;
    }
}

/*!
 * @brief Private function used in conjunction with bst_get_node. Read description there
 * for more information.
 * @param bst[in] bst_t
 * @param node[in] node_t
 * @param target_payload[in] node_payload_t
 * @return Returns the node that has been found, otherwise it returns a NULL
 */
static node_t * search_node(bst_t * bst, node_t * node, node_payload_t * target_payload)
{
    // if the current node is NULL, then just return it
    if (NULL == node)
    {
        return NULL;
    }

    // compare the current node to the target payload
    bst_compare_t result = bst->compare(node->key, target_payload);

    // check the result, and either recurse or return
    if (BST_EQ == result)
    {
        return node;
    }
    else if (BST_LT == result)
    {
        return search_node(bst, node->left_child, target_payload);
    }
    else
    {
        return search_node(bst, node->right_child, target_payload);
    }
}
