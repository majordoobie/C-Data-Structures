#include <bst.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

int8_t global_result = 0;

struct node {
    int height;
    struct node * left_child;
    struct node * right_child;
    node_payload_t * key;
};

// Private insert/deletion functions
static node_t * create_node(node_payload_t * payload);
static node_t * insert_node(node_t * node, node_payload_t * payload, bst_replace_t replace, bst_status_t (*callback)(node_payload_t *, void *, void *), void *ptr, avl_tree_t * bst);
static node_t * get_node(avl_tree_t * bst, node_t * node, node_payload_t * target_payload);
static node_t * remove_node(node_t * node, node_payload_t * payload, avl_tree_t * bst);
static void free_all_nodes(avl_tree_t * bst, node_t * node, bst_destroy_t free_payload);

// Private rotation functions
static node_t * balance_tree(node_t * node);
static node_t * right_rotation(node_t * node);
static node_t * left_rotation(node_t * node);

// Rotation Helper Functions
static int get_balance_factor(node_t * node);
static void set_height(node_t * node);
static int get_height(node_t * node);
static bool is_right_heavy(node_t * node);
static bool is_left_heavy(node_t * node);
static node_t * find_max_payload(node_t * node);
static node_t * find_min_payload(node_t * node);
static int node_max(int left, int right);

// Iterable and print functions
static void print_2d_iter(node_t * node, int space, void (* callback)(node_payload_t *));
static bst_recurse_t in_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr);
static bst_recurse_t pre_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr);
static bst_recurse_t post_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr);


/*!
 * @brief Public function used to printout the binary tree in a way that can visualized.
 * The functions takes a callback functions used to pass the actual payloads to be printed.
 *
 * The callback function should be a simple printf("%d ", node->value);
 * @param bst[in] avl_tree_t
 * @param callback[in] Call back function used to call on each node.
 */
void print_2d(avl_tree_t * bst, void (*callback)(node_payload_t *))
{
    print_2d_iter(bst->root, 0, callback);
}

/*!
 * @brief Initialize the tree structure to include the payload file_count of the node keys.
 * The bst structure also will also require two callback functions. One function to perform
 * comparisons and another function to free the payloads.
 *
 * @param compare[in] Comparison callback functions
 * @param free_payload[in] Payload freeing function
 * @return Pointer to the tree control block
 */
avl_tree_t * bst_init(bst_compare_t (* compare)(node_payload_t *, node_payload_t *), void (* free_payload)(node_payload_t *))
{
    avl_tree_t * bst = calloc(1, sizeof(* bst));
    if (NULL == bst)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes for avl_tree_t.\n", sizeof(* bst));
        abort();
    }
    bst->compare = compare;
    bst->free_payload = free_payload;
    return bst;
}

/*!
 * @brief Public function to destroy the binary tree with the option to either free the
 * payloads or leave them alone.
 * @param bst[in] avl_tree_t
 * @param free_payload[in] Flag used to either destroy or preserve the payloads in the
 * nodes. The "True" will call the payload_free callback used when registering the bst
 */
void bst_destroy(avl_tree_t * bst, bst_destroy_t free_payload)
{
    free_all_nodes(bst, bst->root, free_payload);
    free(bst);
    bst = NULL;
}

/*!
 * @brief Public function used to remove a node with the option to either preserver or
 * free the payload itself.
 *
 * @param bst[in] avl_tree_t
 * @param payload[in] node_payload_t
 * @param free_payload Flag to either free or preserver payload
 * @return
 */
bst_status_t bst_remove(avl_tree_t * bst, node_payload_t * payload, bst_destroy_t free_payload)
{
    // Attempt to fetch the payload passed in, if not found return not found
    node_payload_t * node_payload = bst_get_node(bst, payload);
    if (NULL == node_payload)
    {
        return BST_NODE_NOT_FOUND;
    }

    // If payload is in the tree, remove it and set the new root in case we have a
    // rotation
    bst->root = remove_node(bst->root, payload, bst);

    // Free payload if flag is set to true
    if (FREE_PAYLOAD_TRUE == free_payload)
    {
        bst->free_payload(node_payload);
        global_result = 1;
    }

    // Check if operation was successful
    if (1 == global_result)
    {
        global_result = 0;
        return BST_REMOVE_SUCCESS;
    }
    else
    {
        return BST_REMOVE_FAILURE;
    }
}

/*!
 * @brief Create a new node with the given node_payload. Iterate over the tree using the
 * compare function supplied to the BST object and push the new node onto the tree.
 *
 * @param bst[in] avl_tree_t tree pointer
 * @param payload[in] node_payload_t payload used for the new node
 * @param replace[in] Option to replace or ignore equivalent nodes
 */
bst_status_t bst_insert(avl_tree_t *bst, node_payload_t *payload, bst_replace_t replace, bst_status_t (* callback)(node_payload_t *, void *, void *), void * ptr)
{
    bst->root = insert_node(bst->root, payload, replace, callback, ptr, bst);

    if (1 == global_result)
    {
        global_result = 0;
        return BST_INSERT_SUCCESS;
    }
    else
    {
        return BST_INSERT_FAILURE;
    }
}

/*!
 * @brief Public function to fetch a payload from a node.
 *
 * @param bst[in] avl_tree_t
 * @param payload[in] node_payload_t You must create a payload with at least the collation_value
 * data initialized to do the comparisons
 * @return Found nodes payload or NULL
 */
node_payload_t * bst_get_node(avl_tree_t * bst, node_payload_t * payload)
{
    node_t * node = get_node(bst, bst->root, payload);
    if (NULL == node)
    {
        return NULL;
    }
    return node->key;
}

/*!
 * @brief Traverses the tree and calls the callback function passed in using
 * the current node. Traversal stops when a bst_recurse_t is used or when
 * the tree is done.
 *
 * The function iterates over all the nodes using the traversal algorithm
 * specified. Additionally, after each node is fetch, the node is then passed
 * to the callback function along with the optional void_ptr.
 *
 * The callback function will return a bst_recurse_t object which will tell
 * the traversal function to either continue or stop recursive. When the tree
 * is exhausted, the function will also stop recursion.
 *
 * Example Use Case:
 * Get all the positive node values into a list. Create a callback function
 * that will inspect the value and save the data in the node into the void_ptr.
 * At the end you will have a void_ptr with all the values you were looking for.
 *
 * @param bst avl_tree_t
 * @param type Type of traversal [TRAVERSAL_IN_ORDER, TRAVERSAL_OUT_ORDER,
 *             TRAVERSAL_PRE_ORDER]
 * @param callback Pointer to external supplied function for node processing
 * @param void_ptr Optional void ptr
 */
void bst_traversal(avl_tree_t * bst, bst_traversal_t type, bst_recurse_t (* callback)(node_payload_t *, void *), void * void_ptr)
{
    switch (type)
    {
        case TRAVERSAL_IN_ORDER:
            in_order_traversal_node(bst->root, RECURSE_TRUE, callback, void_ptr);
            break;
        case TRAVERSAL_POST_ORDER:
            post_order_traversal_node(bst->root, RECURSE_TRUE, callback, void_ptr);
            break;
        case TRAVERSAL_PRE_ORDER:
            pre_order_traversal_node(bst->root, RECURSE_TRUE, callback, void_ptr);
            break;
        default:
            break;
    }
}

/*!
 * @brief Create a node object with the passed in payload
 * @param payload[in] node_payload_t
 * @return New node
 */
static node_t * create_node(node_payload_t * payload)
{
    // create a new_node object
    node_t * new_node = calloc(1, sizeof(* new_node));
    if (NULL == new_node)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(* new_node));
        abort();
    }
    new_node->key = payload;
    return new_node;
}

/*!
 * @brief Insert the payload into a newly created node and add it to the tree. If the
 * node already exists, check if the replace flag is set to True, if so, replace payload
 * in the node.
 *
 * Additionally, if more granular control is needed for adding matched nodes, you have
 * the option of providing a callback function along with a void pointer to whatever
 * structure is needed
 *
 * @param node[in] node_t
 * @param payload[in] node_payload_t
 * @param replace[in] bst_replace_t
 * @param callback[in] Pointer to a callback function for the option to do with a already matched node
 * @param ptr[in] NULL pointer for using callback
 * @param bst[in] avl_tree_t
 * @return Return the newest root node. This will change if a rotation is needed to
 * maintain the tree balanced
 */
static node_t * insert_node(node_t * node, node_payload_t * payload, bst_replace_t replace, bst_status_t (*callback)(node_payload_t *, void *, void *), void *ptr, avl_tree_t * bst)
{
    // if the current node is NULL, create a node for it
    if (NULL == node)
    {
        global_result = 1;
        if (NULL == callback)
        {
            return create_node(payload);
        }
        else
        {
            // if callback exists, call it to determine if node should be created
            bst_status_t result = callback(payload, NULL, ptr);
            if (BST_INSERT_SUCCESS == result)
            {
                return create_node(payload);
            }
            else
            {
                global_result = 0;
                return NULL;
            }
        }
    }

    // run the comparison function supplied
    bst_compare_t result = bst->compare(node->key, payload);

    // Recurse until a proper location is found or a match is found
    if (BST_LT == result)
    {
        node->left_child = insert_node(node->left_child, payload, replace, callback, ptr, bst);
    }
    else if (BST_GT == result)
    {
        node->right_child = insert_node(node->right_child, payload, replace, callback, ptr, bst);
    }
    else if (BST_EQ == result)
    {
        global_result = 1;

        if (REPLACE_PAYLOAD_TRUE == replace)
        {
            if (NULL == callback)
            {
                bst->free_payload(node->key);
                node->key = payload;
            }
            else
            {
                //TODO: Leak is here
                callback(node->key, payload, ptr);
            }
        }
    }

    set_height(node);
    node = balance_tree(node);

    return node;
}

/*!
 * @brief Private iterative function to find the node payload that matches the payload
 * passed in.
 *
 * @param bst[in] avl_tree_t
 * @param node[in] node_t
 * @param target_payload[in] node_payload_t
 * @return Found node or NULL
 */
static node_t * get_node(avl_tree_t * bst, node_t * node, node_payload_t * target_payload)
{
    bst_compare_t result;
    while (NULL != node)
    {
        result = bst->compare(node->key, target_payload);

        if (BST_EQ == result)
        {
            //TODO: Add a way to call a callback function with a NULL pointer
            return node;
        }
        else if (BST_LT == result)
        {
            node = node->left_child;
        }
        else if (BST_GT == result)
        {
            node = node->right_child;
        }
    }
    // Return NULL when node is not found
    return NULL;
}

/*
 * @brief Recursively move through the nodes until a match is found. Once it is found,
 * return the node to be freed.
 *
 * @param node[in] node_t
 * @param payload[in] node_payload_t
 * @param bst[in] avl_tree_t
 * @return Return the new root if rotation occurred
 */
static node_t * remove_node(node_t * node, node_payload_t * payload, avl_tree_t * bst)
{
    if (NULL == node)
    {
        return NULL;
    }

    bst_compare_t result = bst->compare(node->key, payload);

    if (BST_LT == result)
    {
        node->left_child = remove_node(node->left_child, payload, bst);
    }
    else if (BST_GT == result)
    {
        node->right_child = remove_node(node->right_child, payload, bst);
    }
    else if (BST_EQ == result)
    {
        /*
         * The following two IF statements check if the left or right node are NULL. If
         * they are, return the opposite. Returning will assign that node to one of the
         * two if statements above i.e. "node->right_child = remove();"
         */
        if ((NULL == node->left_child) || (NULL == node->right_child))
        {
            // set global to 1 meaning that removal was a success
            global_result = 1;

            if (NULL == node->right_child)
            {
                node_t * child_node = node->left_child;
                free(node);
                return child_node;
            }
            else
            {
                node_t * child_node = node->right_child;
                free(node);
                return child_node;

            }
        }
        else
        {
            /*
             * This section will return the NEW child node for the functions ABOVE. If
             * either child node of the current node is NULL, it will return the opposite
             * making that node the new child of one of the parent nodes above.
             *
             * To correctly promote a node, you either want the largest node value
             * on the current nodes left subtree or the smallest node to the right
             * subtree. The approach will instead check the height to make sure the tree
             * is balanced.
             * https://github.com/williamfiset/Algorithms/blob/a6e72cd6c1c7e5a4fde916c4140d6d2b4076762e/src/main/java/com/williamfiset/algorithms/datastructures/balancedtree/AVLTreeRecursive.java#L291
             */
            if (node->left_child->height > node->right_child->height)
            {
                // Find the MAX RIGHT value starting from the current nodes LEFT
                node_t * promote_node = find_max_payload(node->left_child);

                // set new payload to node
                node->key = promote_node->key;

                // Seek out the new collation_value we set to delete the old one
                node->left_child = remove_node(node->left_child, node->key, bst);
            }
            else
            {
                // Find the MAX LEFT value starting from the current nodes RIGHT
                node_t * promote_node = find_min_payload(node->right_child);

                // set new payload to node
                node->key = promote_node->key;

                // Seek out the new collation_value we set to delete the old one
                node->right_child = remove_node(node->right_child, node->key, bst);
            }

        }
    }
    set_height(node);
    node = balance_tree(node);
    return node;

}

/*!
 * @brief Recursion function to free each node
 *
 * @param bst[in] avl_tree_t
 * @param node[in] node_t
 * @param free_payload[in] bst_status_t should include either FREE_PAYLOAD_FALSE or TRUE
 */
static void free_all_nodes(avl_tree_t * bst, node_t * node, bst_destroy_t free_payload)
{
    if (NULL == node)
    {
        return;
    }
    free_all_nodes(bst, node->left_child, free_payload);
    free_all_nodes(bst, node->right_child, free_payload);
    if (FREE_PAYLOAD_TRUE == free_payload)
    {
        bst->free_payload(node->key);
        free(node);
    }
    else if (FREE_PAYLOAD_FALSE == free_payload)
    {
        free(node);
    }
}

/*!
 * @brief Function uses node heights to know when and how to rotate the tree to balance it
 *
 * @param node[in] node_t
 * @return The node passed in with its new rotation orientation
 */
static node_t * balance_tree(node_t * node)
{
    if (is_left_heavy(node))
    {
        // check if a left rotation is required before right
        if (get_balance_factor(node->left_child) < 0)
        {
            node->left_child = left_rotation(node->left_child);
        }
        // always perform a right rotation
        return right_rotation(node);
    }

    else if (is_right_heavy(node))
    {
        // check if right rotation is needed before left
        if (get_balance_factor(node->right_child) > 0)
        {
            node->right_child = right_rotation(node->right_child);
        }
        // always perform a left rotation
        return left_rotation(node);
    }

    // if execution gets here, then the tree is already balanced
    return node;
}

/*!
 * @brief Perform right rotation on a node
 *
 * @param node[in] node_t
 * @return The new node in the current position
 */
static node_t * right_rotation(node_t * node)
{
    node_t * new_root = node->left_child;
    node->left_child = new_root->right_child;
    new_root->right_child = node;

    set_height(node);
    set_height(new_root);
    return new_root;
}

/*!
 * @brief Perform left rotation on a node
 *
 * @param node[in] node_t
 * @return The new node in the current position
 */
static node_t * left_rotation(node_t * node)
{
    node_t * new_root = node->right_child;
    node->right_child = new_root->left_child;
    new_root->left_child = node;

    set_height(node);
    set_height(new_root);
    return new_root;
}

/*!
 * @brief Private function to get the height of the current node while checking for nulls
 *
 * @param node[in] node_t
 * @return Node balance factor value
 */
static int get_balance_factor(node_t * node)
{
    return (NULL == node) ? 0 : get_height(node->left_child) - get_height(node->right_child);
}

/*!
 * @brief Set the height of the nodes
 *
 * @param node[in] node_t
 */
static void set_height(node_t * node)
{
    node->height = node_max(
        get_height(node->left_child),
        get_height(node->right_child)
    ) + 1;
}

/*!
 * @brief Function is used to check for null or return height value
 *
 * @param node[in] node_t
 * @return return the height of a node
 */
static int get_height(node_t * node)
{
    if (NULL == node)
    {
        return -1;
    }
    else
    {
        return node->height;
    }
}


static bool is_right_heavy(node_t * node)
{
    return (get_balance_factor(node) < -1);
}
static bool is_left_heavy(node_t * node)
{
    return (get_balance_factor(node) > 1);
}

/*!
 * @brief Iterate through the right side of given node until NULL is reached then return
 * the collation_value of that node
 * @param node[in] node_t
 * @return Returns the node_payload_t of the last node on the right side of a node given
 */
static node_t * find_max_payload(node_t * node)
{
    while (NULL != node->right_child)
    {
        node = node->right_child;
    }
    return node;
}

/*!
 * @brief Iterate through the right side of given node until NULL is reached then return
 * the collation_value of that node
 * @param node[in] node_t
 * @return Returns the node_payload_t of the last node on the right side of a node given
 */
static node_t * find_min_payload(node_t * node)
{
    while (NULL != node->left_child)
    {
        node = node->left_child;
    }
    return node;
}

static int node_max(int left, int right)
{
    return (left > right) ? left : right;
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

/*!
 * @brief Recursion function to travel through the tree
 * @param node[in] The current node being inspected
 * @param function[in] External function to call on each node
 */
static bst_recurse_t in_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return recurse;
    }

    if (RECURSE_TRUE == recurse)
    {
        in_order_traversal_node(node->left_child, recurse, function, void_ptr);
    }

    if (RECURSE_TRUE == recurse)
    {
        recurse = function(node->key, void_ptr);
    }

    if (RECURSE_TRUE == recurse)
    {
        recurse = in_order_traversal_node(node->right_child, recurse, function, void_ptr);
    }

    return recurse;
}



/*!
 * @brief Recursion function to travel through the tree
 * @param node[in] The current node being inspected
 * @param function[in] External function to call on each node
 */
static bst_recurse_t pre_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return recurse;
    }
    if (RECURSE_TRUE == recurse)
    {
        recurse = function(node->key, void_ptr);
    }

    if (RECURSE_TRUE == recurse)
    {
        recurse = pre_order_traversal_node(node->left_child, recurse, function, void_ptr);
    }

    if (RECURSE_TRUE == recurse)
    {
        recurse = pre_order_traversal_node(node->right_child, recurse, function, void_ptr);
    }
    return recurse;
}

static bst_recurse_t post_order_traversal_node(node_t * node, bst_recurse_t recurse, bst_recurse_t (* function)(node_payload_t *, void *), void * void_ptr)
{
    if (NULL == node)
    {
        return recurse;
    }

    if (RECURSE_TRUE == recurse)
    {
        recurse = post_order_traversal_node(node->left_child, recurse, function, void_ptr);
    }
    if (RECURSE_TRUE == recurse)
    {
        recurse = post_order_traversal_node(node->right_child, recurse, function, void_ptr);
    }
    if (RECURSE_TRUE == recurse)
    {
        recurse = function(node->key, void_ptr);
    }
    return recurse;
}







