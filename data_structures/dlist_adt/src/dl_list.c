#include <stdio.h>
#include <dl_list.h>
#include <stdlib.h>
#include <assert.h>

// If in debug mode, remove static for testing
#ifdef NDEBUG
#define TEST static
#else
#define TEST
#endif

typedef enum
{
    NEXT,
    PREV
} iter_fetch_t;

// Enum for determining if malloc calls were valid
typedef enum
{
    VALID_PTR = 1,
    INVALID_PTR = 0
} valid_ptr_t;

typedef enum
{
    FREE_NODES,
    NO_FREE_NODES,
    APPEND,
    PREPEND,
    INSERT_AT
} dlist_settings_t;


// Structure with the individual satellite data
typedef struct node
{
    void * data;
    struct node * next;
    struct node * prev;
} dnode_t;

// Manager structure
typedef struct dlist_t
{
    dnode_t * head;
    dnode_t * tail;
    dlist_iter_t * iter;
    int32_t length;
    dlist_match_t (* compare_func)(void *, void *);
} dlist_t;

typedef struct dlist_iter_t
{
    dlist_t * dlist;
    dnode_t * node;
    int32_t index;
} dlist_iter_t;

typedef struct
{
    sort_direction_t direction;
    dlist_compare_t (* compare_func)(void *, void *);
} quick_sort_t;

static dnode_t * init_node(void * data);
static valid_ptr_t verify_alloc(void * ptr);
static void iterate(dlist_iter_t * iter, iter_fetch_t fetch);
static dnode_t * get_value(dlist_t * dlist, void * data);
static void dlist_destroy_(dlist_t * dlist, dlist_settings_t delete, void(*free_func)(void *));
static void * remove_node(dlist_t * dlist, dnode_t * node);
static dlist_result_t add_node(dlist_t * dlist,
                               void * data,
                               dlist_settings_t add_mode,
                               int32_t at_index);
TEST int32_t get_inverse(int32_t value);
static dnode_t * get_at_index(dlist_t * dlist, int32_t index);

static bool do_swap(quick_sort_t * sort, dnode_t * left, dnode_t * right);
static void swap_dnodes(dnode_t * left, dnode_t * right);
static void quick_sort(quick_sort_t * sort, dnode_t * left, dnode_t * right);

static dlist_iter_t * dlist_get_iterable(dlist_t * dlist, iter_start_t pos);
static void dlist_destroy_iter(dlist_iter_t * dlist_iter);

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

    // Initialize the sort structure with the comparison function and direction
    * sort = (quick_sort_t) {
        .compare_func = compare_func,
        .direction    = direction
    };
    quick_sort(sort, dlist->head, dlist->tail);

    // free the sort structure
    free(sort);
}

/*!
 * @brief Public function to check if the dlist is empty
 * @param dlist
 * @return
 */
bool dlist_is_empty(dlist_t * dlist)
{
    assert(dlist);
    return dlist->length == 0;
}

/*!
 * @brief Initialize the dlist with a function to perform the comparisons
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

    * dlist = (dlist_t) {
        .compare_func = compare_func,
        .iter         = dlist_get_iterable(dlist, ITER_HEAD)
    };

    return dlist;
}

/*!
 * @brief Return the number of items in the linked list
 *
 * @param dlist
 * @return
 */
size_t dlist_length(dlist_t * dlist)
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
 * @brief Insert a node at the given iter_index. The new node will maintain the iter_index
 * given in the parameter. If the iter_index is not within the invalid range then
 * an error is returned. A negative iter_index is possible with -1 indicating the
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
 * @brief Creates an iterable object with a start node value of either head
 * or tail based on the iter_start_t position. The value is then extracted by
 * using the dlist_get_value and iterated with the iter call
 *
 * @param dlist
 * @return dlist_iter_t object starting at head or tail
 */
dlist_iter_t * dlist_get_iterable(dlist_t * dlist, iter_start_t pos)
{
    // verify that the dlist is a valid pointer
    assert(dlist);
    dlist_iter_t * iter = (dlist_iter_t *)malloc(sizeof(dlist_iter_t));
    if (INVALID_PTR == verify_alloc(iter))
    {
        return NULL;
    }

    // Init the iter structure and return the pointer to it
    *iter = (dlist_iter_t){
        .node       = (ITER_HEAD == pos) ? dlist->head : dlist->tail,
        .dlist      = dlist,
        .index      = (ITER_HEAD == pos || 0 == dlist->length ) ? 0 : dlist->length - 1
    };
    return iter;
}

/*!
 * @brief Function returns the current value at the current iter_index. For a
 * newly created iter, this will either be the head or the tail based ont he
 * end of the list that the iter object was created with.
 *
 * @param dlist_iter
 * @return Pointer to the node data
 */
void * dlist_get_value(dlist_t * dlist)
{
    assert(dlist);
    return dlist->iter->node->data;
}

/*!
 * @brief Return the current iter_index of the iter object
 * @param dlist
 * @return int32_t iter_index of the iterable
 */
size_t dlist_get_index(dlist_t * dlist)
{
    assert(dlist);
    return dlist->iter->index;
}

/*!
 * @brief Reset the iterable to start with the head of the dlist
 * @param dlist
 */
void dlist_set_head(dlist_t * dlist)
{
    assert(dlist);
    dlist->iter->node = dlist->head;
    dlist->iter->index = 0;
}

/*!
 * @brief Reset the iterable to start with the tail of the dlist
 * @param dlist
 */
void dlist_set_tail(dlist_t * dlist)
{
    assert(dlist);
    dlist->iter->node = dlist->tail;

    // length can never be less than 0. If length is already 0 then that
    // means that the tail IS the head. So we set the iter_index to 0 here
    dlist->iter->index = (0 == dlist->length) ? 0 : dlist->length - 1;
}

/*!
 * @brief Iterates over the iterable and returns the prev node. A NULL is
 * returned if the next node is NULL
 * @param dlist
 * @return Returns the data pointer for the node. If the end of the linked
 * list is reached, then a NULL is returned.
 */
void * dlist_get_prev(dlist_t * dlist)
{
    iterate(dlist->iter, PREV);
    if (NULL != dlist->iter->node)
    {
        return dlist->iter_node;
    }
    return NULL;
}

/*!
 * @brief Iterates over the iterable and returns the next node. A NULL is
 * returned if the next node is NULL
 * @param dlist
 * @return Returns the data pointer for the node. If the end of the linked
 * list is reached, then a NULL is returned.
 */
void * dlist_get_next(dlist_t * dlist)
{
    iterate(dlist, NEXT);
    if (NULL != dlist->iter_node)
    {
        return dlist->iter_node;
    }
    return NULL;
}


/*!
 * @brief Destroy the iterable
 * @param dlist_iter
 */
void dlist_destroy_iter(dlist_iter_t * dlist_iter)
{
    free(dlist_iter);
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
 * @brief Fetch the node in the dlist by matching the the value using the
 * comparison function massed in.
 * @param dlist
 * @param data
 * @return Pointer to the node if found else NULL
 */
void * dlist_get_by_value(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    dnode_t * node = get_value(dlist, data);
    if (NULL == node)
    {
        return NULL;
    }
    else
    {
        return node->data;
    }
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
    dnode_t * node = get_value(dlist, data);
    if (NULL == node)
    {
        return NULL;
    }

    return remove_node(dlist, node);
}

/*!
 * @brief Check if value is in the dlist
 * @param dlist
 * @param data
 * @return
 */
dlist_match_t dlist_in_dlist(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    dnode_t * node = get_value(dlist, data);
    if (NULL == node)
    {
        return DLIST_MISS_MATCH;
    }
    return DLIST_MATCH;
}

/*!
 * @brief Check if allocation is valid
 * @param ptr Any pointer
 * @return valid_ptr_t : VALID_PTR or INVALID_PTR
 */
static valid_ptr_t verify_alloc(void * ptr)
{
    if (NULL == ptr)
    {
        fprintf(stderr, "[!] Invalid allocation\n");
        return INVALID_PTR;
    }
    return VALID_PTR;
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
 * @brief Iterate the iter object to the next value specified by `fetch`.
 *
 * @param iter Stand alone iter object or the iter object built into dlist
 * @param fetch
 */
static void iterate(dlist_iter_t * iter, iter_fetch_t fetch)
{
    // If there is no items in the linked list then return  NULL
    if (0 == iter->dlist->length)
    {
        return;
    }

    // Iterate to the next value. If the next value is out of bounds then set
    // te index to -1 indicating an invalid iteration
    if (NEXT == fetch)
    {
        iter->node = iter->node->next;
        iter->index = (++iter->index == iter->dlist->length) ? -1 : iter->index;
    }
    else if(PREV == fetch)
    {
        iter->node = iter->node->prev;
        iter->index--;
    }
}

/*!
 * @brief Get a dlist value by its iter_index in the linked list
 * @param dlist
 * @param index
 * @return void * pointer to the data
 */
void * dlist_get_by_index(dlist_t * dlist, int32_t index)
{
    dnode_t * node = get_at_index(dlist, index);
    if (NULL == node)
    {
        return NULL;
    }
    return node->data;
}


/*
 *
 *
 * START Iteration Section
 *
 *
 *
 */

dlist_iter_t * dlist_get_iterable(dlist_t * dlist, iter_start_t pos);
void * iter_get_value(dlist_iter_t * iter);
void * iter_get_next(dlist_iter_t * iter);
void * iter_get_prev(dlist_iter_t * iter);
void * iter_set_head(dlist_iter_t * iter);
void * iter_set_tail(dlist_iter_t * iter);
uint32_t iter_get_index(dlist_iter_t * iter);

/*
 *
 *
 * END Iteration Section
 *
 *
 *
 */

/*!
 * @brief Handles finding the node in the linked list by comparing it with
 * the values passed in and using the comparison callback function. If found,
 * return the dnode_t else, NULL
 * @param dlist
 * @param data
 * @return
 */
static dnode_t * get_value(dlist_t * dlist, void * data)
{
    if (NULL == dlist->compare_func)
    {
        return NULL;
    }
    // Store the current iter before searching for the target value
    int32_t stored_index = dlist->iter_index;
    dnode_t * stored_node = dlist->iter_node;
    dlist_set_head(dlist);

    while (NULL != dlist->iter_node)
    {
        if (DLIST_MATCH == dlist->compare_func(dlist->iter_node->data, data))
        {
            break;
        }
        iterate(dlist, NEXT);
    }

    dnode_t * found_node = dlist->iter_node;
    dlist->iter_node = stored_node;
    dlist->iter_index = stored_index;

    return found_node;
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


    // get the nodes related to the node being removed
    dnode_t * parent_node = node->prev;
    dnode_t * child_node = node->next;

    /*
     * Update the head and tail values if they are affected by the removal of
     * the node
     */
    if (dlist->head == node)
    {
        dlist->head = child_node;
    }

    // update the tail if affected
    if (dlist->tail == node)
    {
        dlist->tail = parent_node;
    }


    /*
     * Make the parent and child pointers point at each other officially
     * removing the node from the linked list
     */
    if (NULL != parent_node)
    {
        parent_node->next = child_node;
    }
    if (NULL != child_node)
    {
        child_node->prev = parent_node;
    }

    /*
     * Update the iter node if the iter node is the node being removed. If the
     * index of the iter is 0, the set to the head. Otherwise, set it to the
     * child if not NULL or parent if NULL which requires a decrement of the
     * index.
     */
    if (dlist->iter_node == node)
    {
        // if index is zero, then set the head to iter_node
        if (0 == dlist->iter_index)
        {
            dlist->iter_node = dlist->head;
        }
        else if(NULL == child_node)
        {
            dlist->iter_node = parent_node;
            dlist->iter_index--;
        }
        else
        {
            dlist->iter_node = child_node;
        }

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

    // If length is 0 then this is the first item to add so set the head and
    // tail to the same value.
    if (0 == dlist->length)
    {
        * dlist = (dlist_t){
            .head   = node,
            .tail   = node,
        };
        * dlist->iter = (dlist_iter_t){
            .node   = node,
            .index  = 0
        };
    }
    else if (PREPEND == add_mode)
    {
        // Update the iter pointer
        if (0 == dlist->iter->index)
        {
            dlist->iter->node = node;
        }

        // add node to the linked list
        node->next = dlist->head;
        node->next->prev = node;
        dlist->head = node;

    }
    else if (INSERT_AT == add_mode)
    {
        dnode_t * child_node = get_at_index(dlist, at_index);
        if (NULL == child_node)
        {
            return DLIST_FAIL;
        }

        // update iter iter_index if affected
        if (child_node == dlist->iter->node)
        {
            dlist->iter->node = node;
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

/*!
 * @brief Internal function which fetches the node at the iter_index using a iter
 * object. If the iter_index is invalid an None is returned. If the negative value
 * is the MIN then the same value is returned.
 * k
 * @param dlist
 * @param index
 * @return dnode_t pointer or NULL
 */
static dnode_t * get_at_index(dlist_t * dlist, int32_t index)
{
    // the target iter_index we want to match with
    int32_t target_index = index;
    iter_start_t flag = ITER_HEAD;

    // if we have a positive iter_index, check if its with in the positive range
    if (index > -1)
    {
        if (index > dlist->length - 1)
        {
            return NULL;
        }
    }
    else // If index is negative make sure that we can convert it to a positive
    {
        if (get_inverse(index) > dlist->length)
        {
            return NULL;
        }
        // If we have a valid negative, convert the value to a positive iter_index
        target_index = (int32_t)dlist->length + index;
        flag = ITER_TAIL;
    }

    // create the iter object to start iterating
    dlist_iter_t * iter = dlist_get_iterable(dlist, flag);

    while (target_index != iter->index)
    {
        if (ITER_HEAD == flag)
        {
            iterate(iter, NEXT);
        }
        else
        {
            iterate(iter, PREV);
        }
    }

    dnode_t * node = iter->node;
    dlist_destroy_iter(iter);
    return node;
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
