#include <dl_iter.h>
#include <assert.h>
#include <stdlib.h>

// If in debug mode, remove static for testing
#ifdef NDEBUG
#define TEST static
#else
#define TEST
#endif

typedef struct dlist_iter_t
{
    dlist_t * dlist;
    dnode_t * node;
    int32_t index;
} dlist_iter_t;

TEST int32_t get_inverse(int32_t value);

/*!
 * @brief Create an iterable object. The iterable is a structure capable of
 * iterating through the instance of the dlist. Keep in mind that all iter
 * objects share the same dlist. If you modify one iter object it will affect
 * all iter objects and the dlist.
 *
 * The node parameter is the node to point at. This can be any value. The parent
 * API should set this to either head or tail using an enum and the index is the
 * index of the given  node.
 *
 * The dlist is provided to be able to interact with the dlist such as fetching
 * the dlist length as it changes.
 *
 * @param node
 * @param dlist
 * @param index
 * @return dlist_iter_t or NULL if failure to malloc
 */
dlist_iter_t * iter_get_iterable(dnode_t * node, dlist_t * dlist, int32_t index)
{
    // verify that the dlist is a valid pointer
    assert(dlist);
    dlist_iter_t * iter = (dlist_iter_t *)malloc(sizeof(dlist_iter_t));
    if (INVALID_PTR == verify_alloc(iter))
    {
        return NULL;
    }

    *iter = (dlist_iter_t){
        .node       = node,
        .dlist      = dlist,
        .index      = index
    };

    return iter;
}

/*!
 * @brief Destroy the iter object
 * @param iter
 */
void iter_destroy_iterable(dlist_iter_t * iter)
{
    free(iter);
}

/*!
 * @brief Function returns the current value at the current index. For a
 * newly created iter, this will either be the head or the tail based on the
 * end of the list that the iter object was created with.
 *
 * @param iter
 * @return Pointer to the void data stored at the iter node. If NULL is returned,
 * then the dlist is empty
 */
void * iter_get_value(dlist_iter_t * iter)
{
    assert(iter);
    return iter->node->data;
}

/*!
 * @brief Returns the current index of the current node that the iter object
 * is pointing to
 * @param iter
 * @return Index of the current node. If -1 is returned, then the dlist is empty
 */
int32_t iter_get_index(dlist_iter_t * iter)
{
    return iter->index;
}

/*!
 * @brief Fetches the current node that the iter is pointing to. This function
 * should only be used by the dlist API since the dnode_t
 *
 * @param iter
 * @return Pointer to the dnode_t at the current iter pointer
 */
dnode_t * iter_get_node(dlist_iter_t * iter)
{
    return iter->node;
}

/*!
 * @brief Function sets the node pointer to the node specified. This can be used
 * to reset the node iter back to head, tail, or any other index.
 *
 * @param iter
 * @param node
 * @param index
 */
void iter_set_node(dlist_iter_t * iter, dnode_t * node, int32_t index)
{
    assert(iter);
    assert(node);
    iter->node = node;
    iter->index = index;
}

/*!
 * @brief Function to return the dlist pointer inside the iter. This function
 * should only be used from the dlist API
 *
 * @param iter
 * @return Pointer to the dlist used to create the iter object
 */
dlist_t * iter_get_dlist(dlist_iter_t * iter)
{
    return iter->dlist;
}


/*!
 * @brief Internal function to iterate the iter object. After it updates the
 * iter object it will then return that specific node incase it is needed.
 *
 * @param iter
 * @return Returns the dnode_t object that is on the dlist. This could be
 * NULL if the next node in the list is NULL.
 */
dnode_t * iterate(dlist_iter_t * iter, iter_fetch_t fetch)
{
    // If this is not the initial, check if the iter node is already set to
    // NULL. If it is, then just return
    if (NULL == iter->node)
    {
        return NULL;
    }

    // iterate the value based on the direction
    if (NEXT == fetch)
    {
        iter->node = iter->node->next;
        iter->index++;
    }

    else
    {
        iter->node = iter->node->prev;
        iter->index--;
    }
    return iter->node;
}


/*!
 * @brief Create a search object to aid in the iteration of the linked list
 *
 * @param iter iter object
 * @param data
 * @param index
 * @param search_by Search type to perform: SEARCH_BY_INDEX || SEARCH_BY_VALUE
 * @return Pointer to the search structure
 */
iter_search_t * iter_init_search(dlist_iter_t * iter,
                                 void * data,
                                 int32_t index,
                                 iter_search_by search_by)
{
    iter_search_t * search = (iter_search_t *)malloc(sizeof(iter_search_t));
    if (INVALID_PTR == verify_alloc(search))
    {
        return NULL;
    }

    * search = (iter_search_t){
        .iter           = iter,
        .search_by      = search_by,
        .target_data    = data,
        .target_index   = index,
        .found_node     = NULL,
        .found_index    = -1,
    };
    return search;
}

/*!
 * @brief Destroy the iter object
 * @param search
 */
void iter_destroy_search(iter_search_t * search)
{
    free(search);
}


/*!
 * @brief Wrapper that creates a iter and search object and performs the search
 * based on the index. The function will then cleanup after itself
 *
 * @param dlist
 * @param index
 * @return Pointer to the dnode_t in the dlist or NULL if it was not found
 */
dnode_t * iter_search_by_index(dlist_t * dlist, int32_t index)
{
    assert(dlist);

    // Create iter and search structures
    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
    iter_search_t * search = iter_init_search(iter, NULL, index, SEARCH_BY_INDEX);

    // Perform the search
    iter_search(search);

    // Fetch found data and return
    dnode_t * found_data = search->found_node;
    iter_destroy_search(search);
    iter_destroy_iterable(iter);
    return found_data;
}

iter_search_t * iter_search_by_value_plus(dlist_t * dlist, void * data)
{
    // Create iter and search structures
    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
    iter_search_t * search = iter_init_search(iter, data, 0, SEARCH_BY_VALUE);

    // Perform the search
    iter_search(search);

    // Fetch found data and return
    iter_destroy_iterable(iter);

    return search;
}
/*!
 * @brief Wrapper that creates a iter and search object and performs the search
 * based on the index. The function will then cleanup after itself
 *
 * @param dlist
 * @param data
 * @return Pointer to the dnode_t in the dlist or NULL if it was not found
 */
dnode_t * iter_search_by_value(dlist_t * dlist, void * data)
{
    iter_search_t * search = iter_search_by_value_plus(dlist, data);

    dnode_t * found_data = search->found_node;
    iter_destroy_search(search);
    return found_data;
}

/*!
 * @brief Perform an iter search based on the search structure. The function
 * is capable of conversing a negative value into the proper index. If a negative
 * is used then a reverse iteration is performed.
 *
 * @param search
 * @return SEARCH_SUCCESS or SEARCH_FAILURE
 */
iter_search_result iter_search(iter_search_t * search)
{
    assert(search);
    size_t dlist_length = dlist_get_length(search->iter->dlist);
    iter_fetch_t iterate_to = NEXT;

    // If search by index, ensure that the index is within range
    if (SEARCH_BY_INDEX == search->search_by)
    {
        // If index is a positive integer
        if (search->target_index > -1)
        {
            // If the ABS of index is grater than our length, fail
            if ((size_t)search->target_index > (dlist_length - 1))
            {
                return SEARCH_FAILURE;
            }
        }

        else // Target index is a negative integer
        {
            // If the inverse is greater than our length, return NULL
            if ((size_t)get_inverse(search->target_index) > dlist_length)
            {
                return SEARCH_FAILURE;
            }

            // If negative index is valid, convert it to a positive and change
            // the direction of the search. This is useful for when wanting
            // to search for an index from the end of the linked list
            search->target_index = (int32_t)dlist_length + search->target_index;
            iterate_to = PREV;
            dlist_set_iter_tail(search->iter);
        }
    }


    dnode_t * current_node = iter_get_node(search->iter);
    int32_t current_index = iter_get_index(search->iter);
    dlist_match_t (* compare_func)(void *, void *) = get_func(search->iter->dlist);

    while (NULL != current_node)
    {
        if (SEARCH_BY_VALUE == search->search_by)
        {
            if (DLIST_MATCH == compare_func(current_node->data, search->target_data))
            {
                search->found_node = current_node;
                search->found_index = current_index;
                return SEARCH_SUCCESS;
            }
        }
        else   // Else SEARCH_BY_INDEX
        {
            if (current_index == search->target_index)
            {
                search->found_node = current_node;
                search->found_index = current_index;
                return SEARCH_SUCCESS;
            }
        }


        // Iterate and update the values
        iterate(search->iter, iterate_to);
        current_node = iter_get_node(search->iter);
        current_index = iter_get_index(search->iter);
    }

    return SEARCH_FAILURE;
}


/*!
 * @brief Function returns the inverse of a number. The function will return
 * the same value of a int 32 min because there is no inverse for that value
 * in a 32 bit number
 * @param value
 * @return Inverse of the number or same number for INT32_MIN
 */
TEST int32_t get_inverse(int32_t value)
{
    if (0 == value)
    {
        return value;
    }

    // we have a positive number, return a negative
    if (value > -1)
    {
        // if the value is maxed, then return the min value plus 1
        if (INT32_MAX == value)
        {
            return INT32_MIN + 1;
        }

        return -value;
    }
    else
    {
        // if the value is min, then there is no inverse possible so return
        // the same value back
        if (INT32_MIN == value)
        {
            return value;
        }
        else
        {
            return -value;
        }
    }
}
