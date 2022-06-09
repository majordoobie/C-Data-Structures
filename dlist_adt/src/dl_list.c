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
    PREPEND
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
    size_t length;
    dlist_match_t (* compare_func)(void *, void *);
} dlist_t;

typedef struct dlist_iter_t
{
    dlist_t * dlist;
    dnode_t * node;
    int32_t index;
    size_t length;
} dlist_iter_t;

static dnode_t * init_node(void * data);
static valid_ptr_t verify_alloc(void * ptr);
static dnode_t * iterate(dlist_iter_t * iter, iter_fetch_t fetch);
static dnode_t * get_value(dlist_t * dlist, void * data);
static void dlist_destroy_(dlist_t * dlist, dlist_settings_t delete, void(*free_func)(void *));
static void * remove_node(dlist_t * dlist, dnode_t * node);
static void add_node(dlist_t * dlist, void * data, dlist_settings_t add_mode);
TEST int32_t get_inverse(int32_t value);



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
 * @return
 */
dlist_t * dlist_init(dlist_match_t (* compare_func)(void *, void *))
{
    dlist_t * dlist = (dlist_t *)calloc(1, sizeof(dlist_t));
    verify_alloc(dlist);
    dlist->compare_func = compare_func;
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
 * @brief Prepend a object to the end of the linked list
 *
 * @param dlist
 * @param data
 */
void dlist_prepend(dlist_t * dlist, void * data)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    add_node(dlist, data, PREPEND);
}

/*!
 * @brief Append an item to the end of the linked list
 *
 * @param dlist
 * @param data
 */
void dlist_append(dlist_t * dlist, void * data)
{
    // make sure that the pointers are valid
    assert(dlist);
    assert(data);

    add_node(dlist, data, APPEND);
}

/*!
 * @brief Creates an iterable object with a start node value of either head
 * or tail based on the iter_start_t position. The value is then extracted by
 * using the dlist_get_iter_value and iterated with the iter call
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
        .length     = dlist->length,
        .index      = (ITER_HEAD == pos || 0 == dlist->length ) ? 0 :
                                                    (int32_t)dlist->length - 1
    };
    return iter;
}

/*!
 * @brief Function returns the current value at the current index. For a
 * newly created iter, this will either be the head or the tail based ont he
 * end of the list that the iter object was created with.
 *
 * @param iter
 * @return Pointer to the node data
 */
void * dlist_get_iter_value(dlist_iter_t * iter)
{
    assert(iter);
    return iter->node->data;
}

/*!
 * @brief Return the current index of the iter object
 * @param iter
 * @return int32_t index of the iterable
 */
int32_t dlist_get_iter_index(dlist_iter_t * iter)
{
    assert(iter);
    return iter->index;
}

/*!
 * @brief Reset the iterable to start with the head of the dlist
 * @param iter
 */
void dlist_set_iter_head(dlist_iter_t * iter)
{
    assert(iter);
    iter->node = iter->dlist->head;
    iter->index = 0;
}

/*!
 * @brief Reset the iterable to start with the tail of the dlist
 * @param iter
 */
void dlist_set_iter_tail(dlist_iter_t * iter)
{
    assert(iter);
    iter->node = iter->dlist->tail;

    // length can never be less than 0. If length is already 0 then that
    // means that the tail IS the head. So we set the index to 0 here
    iter->length = (0 == iter->length) ? 0 : iter->length - 1;
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
 * @return
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
 * @return valid_ptr_t
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
 * @brief Internal function to iterate the iter object. After it updates the
 * iter object it will then return that specific node incase it is needed.
 *
 * @param iter
 * @return Returns the dnode_t object that is on the dlist. This could be
 * NULL if the next node in the list is NULL.
 */
static dnode_t * iterate(dlist_iter_t * iter, iter_fetch_t fetch)
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
 * @brief Get a dlist value by its index in the linked list
 * @param dlist
 * @param index
 * @return void * pointer to the data
 */
void * dlist_get_by_index(dlist_t * dlist, int32_t index)
{
    // the target index we want to match with
    int32_t target_index = index;
    iter_start_t flag = ITER_HEAD;

    // if we have a positive index, check if its with in the positive range
    if (index > -1)
    {
        if ((size_t)index > dlist->length - 1)
        {
            return NULL;
        }
    }
    else
    {
        if ((size_t)get_inverse(index) > dlist->length)
        {
            return NULL;
        }
        // If we have a valid negative, convert the value to a positive index
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

    void * data = dlist_get_iter_value(iter);
    dlist_destroy_iter(iter);
    return data;
}

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

    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
    dnode_t * node;
    dlist_match_t found = DLIST_MISS_MATCH;
    while (NULL != (node = iterate(iter, NEXT)))
    {
        if (DLIST_MATCH == dlist->compare_func(node->data, data))
        {
            found = DLIST_MATCH;
            break;
        }
    }

    // destroy the iter and return the value if found
    dlist_destroy_iter(iter);
    if (DLIST_MISS_MATCH == found)
    {
        return NULL;
    }
    else
    {
        return node;
    }
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
static void add_node(dlist_t * dlist, void * data, dlist_settings_t add_mode)
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
    else
    {
        node->prev = dlist->tail;
        node->prev->next = node;
        dlist->tail = node;
    }
    dlist->length++;
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

