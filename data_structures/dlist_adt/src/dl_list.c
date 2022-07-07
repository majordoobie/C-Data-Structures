#include <stdio.h>
#include <dl_list.h>
#include <stdlib.h>
#include <assert.h>
#include <dl_iter.h>

// Settings are used to reduce code complexity by setting a action flag
typedef enum
{
    FREE_NODES,
    NO_FREE_NODES,
    APPEND,
    PREPEND,
    INSERT_AT
} dlist_settings_t;


// Manager structure
typedef struct dlist_t
{
    dnode_t * head;
    dnode_t * tail;
    size_t length;
    dlist_match_t (* compare_func)(void *, void *);
} dlist_t;


typedef struct
{
    sort_direction_t direction;
    dlist_compare_t (* compare_func)(void *, void *);
} quick_sort_t;


// Sorting functions
static bool do_swap(quick_sort_t * sort, dnode_t * left, dnode_t * right);
static void swap_dnodes(dnode_t * left, dnode_t * right);
static void quick_sort(quick_sort_t * sort, dnode_t * left, dnode_t * right);

// Private fetch
static dnode_t * get_by_index(dlist_t * dlist, int32_t index);

// Node private functions
static void dlist_destroy_(dlist_t * dlist, dlist_settings_t delete, void(*free_func)(void *));
static dnode_t * init_node(void * data);
static void * remove_node(dlist_t * dlist, dnode_t * node);
static dlist_result_t add_node(dlist_t * dlist,
                               void * data,
                               dlist_settings_t add_mode,
                               int32_t at_index);



/*!
 * @brief Initialize the dlist with a function to perform the comparisons. The
 * function parameter is used to perform search functionality and must use the
 * dlist_match_t return types
 *
 * @param compare_func
 * @return Null if INVALID_PTR is returned from init or dlist_t pointer
 */
dlist_t * dlist_init(dlist_match_t (* compare_func)(void *, void *))
{
    dlist_t * dlist = (dlist_t *)calloc(1, sizeof(dlist_t));
    valid_ptr_t result = verify_alloc(dlist);
    if (result == INVALID_PTR)
    {
        return NULL;
    }

    dlist->compare_func = compare_func;
    return dlist;
}

/*!
 * @brief Public function to check if the dlist is empty
 * @param dlist
 * @return True if the dlist is empty else False
 */
bool dlist_is_empty(dlist_t * dlist)
{
    assert(dlist);
    return dlist->length == 0;
}

/*!
 * @brief Return the number of items in the linked list
 *
 * @param dlist
 * @return Length of the dlist
 */
size_t dlist_get_length(dlist_t * dlist)
{
    return dlist->length;
}

/*!
 * @brief Insert a node at the head of the linked list making the new node the
 * head node.
 * @param dlist
 * @param data
 */
void dlist_prepend(dlist_t * dlist, void * data)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    add_node(dlist, data, PREPEND, 0);
}

/*!
 * @brief Insert a node at the tail of the linked list making the new node the
 * tail node.
 * @param dlist
 * @param data
 */
void dlist_append(dlist_t * dlist, void * data)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    add_node(dlist, data, APPEND, 0);
}

/*!
 * @brief Insert a node at the given index. The new node will maintain the index
 * given in the parameter. If the index is not within the invalid range then
 * an error is returned. A negative index is possible with -1 indicating the
 * tail node and the absolute value of the negative length of the list minus
 * one indicating the head node.
 * @param dlist
 * @param data
 * @param index
 * @return DLIST_SUCC if successful or DLIST_FAIL
 */
dlist_result_t dlist_insert(dlist_t * dlist, void * data, int32_t index)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    return add_node(dlist, data, INSERT_AT, index);
}

/*!
 * @brief Free the double linked list without freeing the satellite data.
 *
 * This function will leave the satellite data up to the user to free.
 * @param dlist
 * @return
 */
void dlist_destroy(dlist_t * dlist)
{
    assert(dlist);
    dlist_destroy_(dlist, NO_FREE_NODES, NULL);
}

/*!
 * @brief Free the double linked list while also freeing the nodes with the
 * function passed in
 * @param dlist
 * @param free_func
 */
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *))
{
    assert(dlist);
    if (NULL == free_func)
    {
        fprintf(stderr, "[!] Invalid free function pointer passed\n");
        return;
    }
    dlist_destroy_(dlist, FREE_NODES, free_func);
}

/*!
 * @brief Function to pop values from the tail
 * @param dlist
 * @return
 */
void * dlist_pop_tail(dlist_t * dlist)
{
    assert(dlist);
    if (dlist_is_empty(dlist))
    {
        return NULL;
    }

    dnode_t * node = dlist->tail;
    return remove_node(dlist, node);
}

/*!
 * @brief Function to pop values from the head
 * @param dlist
 * @return
 */
void * dlist_pop_head(dlist_t * dlist)
{
    assert(dlist);
    if (dlist_is_empty(dlist))
    {
        return NULL;
    }

    dnode_t * node = dlist->head;
    return remove_node(dlist, node);
}

/*!
 * @brief Remove node from the dlist using the value passed in
 * @param dlist
 * @param data
 * @return NULL if item it not found. Otherwise, the pointer to the data is
 * returned.
 */
void * dlist_remove_value(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    dnode_t * node = iter_search_by_value(dlist, data);
    if (NULL == node)
    {
        return NULL;
    }

    return remove_node(dlist, node);
}

/*!
 * Function is used for the iter API since dlist is opaque
 *
 * @param dlist
 */
dlist_match_t (* get_func(dlist_t * dlist))(void *, void *)
{
    return dlist->compare_func;
}


/*!
 * @brief Check if allocation is valid
 * @param ptr Any pointer
 * @return valid_ptr_t : VALID_PTR or INVALID_PTR
 */
valid_ptr_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        fprintf(stderr, "[!] Invalid allocation\n");
        return INVALID_PTR;
    }
    return VALID_PTR;
}

/*!
 * @brief Perform a quick sort on the double linked list. The quick sort
 * relies on the comparison function passed. The sort does not create any new
 * data structures, the dnode_t data pointers are updated in place. The
 * algorithm sorts in O(n log(n)) in most cases unless the list is already
 * sorted in which case the algorithm will operate in O(n^2)
 *
 * @param dlist
 * @param direction
 * @param compare_func
 */
void dlist_quick_sort(dlist_t * dlist,
                      sort_direction_t direction,
                      dlist_compare_t (* compare_func)(void *, void *))
{
    // Return if the is only one/none items in the linked list
    if (2 > dlist->length)
    {
        return;
    }

    // Create the struct that will make it easier to manage the settings
    quick_sort_t * sort = (quick_sort_t *)malloc(sizeof(quick_sort_t));
    if (INVALID_PTR == verify_alloc(sort))
    {
        return;
    }

    // Initialize the sort structure with the comparison function and direction!
    * sort = (quick_sort_t) {
        .compare_func = compare_func,
        .direction    = direction
    };
    quick_sort(sort, dlist->head, dlist->tail);

    // free the sort structure
    free(sort);
}

/*********************************************************************************************
 *
 *                                Search Section
 *
 * Section uses the iter API to find the value but abstracts the creation of the
 * iter object
 *
 ********************************************************************************************/

/*!
 * @brief Function returns true if the value passed in is found in the dlist
 * else a false is returned
 *
 * @param dlist
 * @param data
 * @return True if data is in dlist else false
 */
bool dlist_value_in_dlist(dlist_t * dlist, void * data)
{
    void * node = dlist_get_by_value(dlist, data);
    if (NULL == node)
    {
        return false;
    }
    return true;
}

/*!
 * @brief Return the data stored in the dlist by matching it with the value
 * passed in.
 *
 * @param dlist
 * @param data
 * @return Pointer to the data stored in the dlist or NULL
 */
void * dlist_get_by_value(dlist_t * dlist, void * data)
{
    // Assert values
    assert(dlist);
    assert(data);

    dnode_t * found_node = iter_search_by_value(dlist, data);

    if (NULL == found_node)
    {
        return NULL;
    }

    return found_node->data;
}

/*!
 * @brief Return the data stored in the dlist by matching with the index passed
 * in
 *
 * @param dlist
 * @param index
 * @return Pointer to the data stored in the dlist or NULL
 */
void * dlist_get_by_index(dlist_t * dlist, int32_t index)
{
    dnode_t * node = get_by_index(dlist, index);
    if (NULL == node)
    {
        return NULL;
    }
    return node->data;
}

/*********************************************************************************************
 *
 *                                 Iter API Section
 *
 * This section uses the iter API to interact with the iter object.
 *
 ********************************************************************************************/

/*!
 * @brief Creates an iterable object with a start node value of either head
 * or tail based on the iter_start_t position. The value is then extracted by
 * using the iter_get_value and iterated with the iter call
 *
 * @param dlist
 * @return dlist_iter_t object starting at head or tail
 */
dlist_iter_t * dlist_get_iterable(dlist_t * dlist, iter_start_t pos)
{
    // verify that the dlist is a valid pointer
    assert(dlist);

    /*
     * The index value is either going to be:
     *  0           : Head when dlist is not empty
     *  -1          : When dlist is empty
     *  length - 1  : When tail
     */
    dlist_iter_t * iter = iter_get_iterable(
        (ITER_HEAD == pos) ? dlist->head : dlist->tail,
        dlist,
        (ITER_HEAD == pos || 0 == dlist->length ) ? 0 : (int32_t)dlist->length - 1
        );
    return iter;
}

/*!
 * @brief Destroy the iterable
 * @param dlist_iter
 */
void dlist_destroy_iter(dlist_iter_t * dlist_iter)
{
    iter_destroy_iterable(dlist_iter);
}

/*!
 * @brief Reset the iterable to start with the head of the dlist
 * @param iter
 */
void dlist_set_iter_head(dlist_iter_t * iter)
{
    assert(iter);

    // Get the embedded dlist from iter
    dlist_t * dlist = iter_get_dlist(iter);
    assert(dlist);

    iter_set_node(iter, dlist->head, 0);
}

/*!
 * @brief Reset the iterable to start with the tail of the dlist
 * @param iter
 */
void dlist_set_iter_tail(dlist_iter_t * iter)
{
    assert(iter);

    // Get the embedded dlist from iter
    dlist_t * dlist = iter_get_dlist(iter);
    assert(dlist);

    // length can never be less than 0. If length is already 0 then that
    // means that the tail IS the head. So we set the index to 0 here
    iter_set_node(
        iter,
        dlist->tail,
        (0 == dlist->length) ? 0 : (int32_t)dlist->length - 1
    );
}

/*!
 * @brief Iterates over the iterable and returns the prev node. A NULL is
 * returned if the next node is NULL
 * @param dlist_iter
 * @return Returns the data pointer for the node. If the end of the linked
 * list is reached, then a NULL is returned.
 */
void * dlist_get_iter_prev(dlist_iter_t * dlist_iter)
{
    dnode_t * data = iterate(dlist_iter, PREV);
    if (NULL != data)
    {
        return data->data;
    }
    return NULL;

}

/*!
 * @brief Iterates over the iterable and returns the next node. A NULL is
 * returned if the next node is NULL
 * @param dlist_iter
 * @return Returns the data pointer for the node. If the end of the linked
 * list is reached, then a NULL is returned.
 */
void * dlist_get_iter_next(dlist_iter_t * dlist_iter)
{
    dnode_t * data = iterate(dlist_iter, NEXT);
    if (NULL != data)
    {
        return data->data;
    }
    return NULL;
}

static dnode_t * get_by_index(dlist_t * dlist, int32_t index)
{
    // Assert values
    assert(dlist);

    dnode_t * found_node = iter_search_by_index(dlist, index);

    if (NULL == found_node)
    {
        return NULL;
    }

    return found_node;
}

/*!
 * @brief Create the structure that is stored on each item in the linked list
 * @param data
 * @return
 */
static dnode_t * init_node(void * data)
{
    dnode_t * node = (dnode_t *)calloc(1, sizeof(dnode_t));
    if (INVALID_PTR == verify_alloc(node))
    {
        return NULL;
    }
    node->data = data;
    return node;
}

/*!
 * @brief Private function handles the removal of the identified node
 * @param dlist
 * @param node
 * @return
 */
static void * remove_node(dlist_t * dlist, dnode_t * node)
{
    // preserve the node data before removing
    void * node_data = node->data;
    dlist->length--;

    // check if node is the head, if so, update
    if (dlist->head == node)
    {
        dlist->head = node->next;
        if (NULL != dlist->head)
        {
            dlist->head->prev = NULL;
        }
    }

    // Check if node is the tail, if so, update
    if (dlist->tail == node)
    {
        dlist->tail = node->prev;
        if (NULL != dlist->tail)
        {
            dlist->tail->next = NULL;
        }
    }

    // Update the poiters for left and right
    if (NULL != node->prev)
    {
        node->prev->next = node->next;
    }
    if (NULL != node->next)
    {
        node->next->prev = node->prev;
    }

    // free the node and return the actual data
    free(node);
    return node_data;
}

/*!
 * @brief Private function that handles the appending or prepending of nodes
 * to the linked list.
 *
 * @param dlist
 * @param data
 * @param add_mode
 */
static dlist_result_t add_node(dlist_t * dlist,
                               void * data,
                               dlist_settings_t add_mode,
                               int32_t at_index)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    dnode_t * node = init_node(data);
    if (NULL == node)
    {
        // if we get here, then something terrible has happened to memory
        // allocation. Clean up as much as possible and abort
        dlist_destroy(dlist);
        abort();
    }
    node->data = data;

    // If tail is None, then we know that there is no items in the linked
    // list. So this item will be the very first item appended
    if (0 == dlist->length)
    {
        dlist->head = node;
        dlist->tail = node;
    }
    else if (PREPEND == add_mode)
    {
        node->next = dlist->head;
        node->next->prev = node;
        dlist->head = node;
    }
    else if (INSERT_AT == add_mode)
    {
        dnode_t * child_node = get_by_index(dlist, at_index);
        if (NULL == child_node)
        {
            return DLIST_FAIL;
        }

        dnode_t * parent_node = child_node->prev;
        child_node->prev = node;
        node->prev = parent_node;
        node->next = child_node;
        parent_node->next = node;
    }
    else
    {
        node->prev = dlist->tail;
        node->prev->next = node;
        dlist->tail = node;
    }
    dlist->length++;
    return DLIST_SUCC;
}



/*
 * Sort functions
 */
/*!
 * @brief Perform a comparison between the two nodes using the function
 * pointer passed in by the caller. Using the result, and the sort direction,
 * return a bool indicating if the values should be swapped by the sort
 * function.
 *
 * When DESCENDING we only want to return true when the comparison is LT
 * (left is LT right) or EQ.
 *
 * When ASCENDING we only want to return true when the comparison is GT (left
 * is GT right) or EQ.
 *
 * @param sort
 * @param left
 * @param right
 * @return Bool indicating if the values should be swapped based on the
 * comparison pointer passed in
 */
static bool do_swap(quick_sort_t * sort, dnode_t * left, dnode_t * right)
{
    // perform a comparison using the function passed in
    dlist_compare_t compare = sort->compare_func(left->data, right->data);

    // When descending, we only want to swap when the compare is LT
    if (DESCENDING == sort->direction)
    {
        return (DLIST_LT == compare) ? false : true;
    }
    else
    {
        return (DLIST_GT == compare) ? false : true;
    }
}
/*!
 * @brief Perform the partition algorithm by iterating from the left node up
 * to the pivot (right) and swap data pointers based on the sort->direction.
 *
 * @param sort
 * @param left
 * @param right
 * @return Return the partition pointer which is guaranteed to be sorted.
 */
static dnode_t * partition(quick_sort_t * sort, dnode_t * left, dnode_t * right)
{
    dnode_t * pivot = right;
    dnode_t * partition_node = left->prev;

    // Index is the value that is iterating over the list upto the pivot
    for (dnode_t * index = left; index != pivot; index = index->next)
    {
        // check if the values should be swapped based on ascending order
        if (do_swap(sort, index, pivot))
        {
            // If a swap is to be called, increment the partition_node by one
            partition_node = (NULL == partition_node) ? left : partition_node->next;
            swap_dnodes(partition_node, index);
        }
    }
    // Finally perform a final swap after the completed iteration
    partition_node = (NULL == partition_node) ? left : partition_node->next;
    swap_dnodes(partition_node, right);
    return partition_node;
}

/*!
 * @brief Swap two dnode_t data pointers
 * @param left
 * @param right
 */
static void swap_dnodes(dnode_t * left, dnode_t * right)
{
    void * left_data = left->data;
    left->data = right->data;
    right->data = left_data;
}


/*!
 * @brief Recursive algorithm to sort the linked list.
 * @param sort
 * @param left
 * @param right
 */
static void quick_sort(quick_sort_t * sort, dnode_t * left, dnode_t * right)
{
    if (right != NULL && left != right && left != right->next)
    {
        dnode_t * partition_node = partition(sort, left, right);

        quick_sort(sort, left, partition_node->prev);
        quick_sort(sort, partition_node->next, right);
    }
}

/*!
 * Static function that handles the actual deletion. If free of the nodes is
 * requested then a function pointer to how to free the nodes is required.
 * @param dlist
 * @param delete
 * @param free_func
 */
static void dlist_destroy_(dlist_t * dlist, dlist_settings_t delete, void (*free_func)(void *))
{
    dnode_t * node = dlist->head;
    dnode_t * next_node;
    while (NULL != node)
    {
        next_node = node->next;
        if (FREE_NODES == delete)
        {
            free_func(node->data);
        }
        free(node);
        node = next_node;
    }
    free(dlist);
}
