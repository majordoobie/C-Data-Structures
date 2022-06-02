#include <stdio.h>
#include <dl_list.h>
#include <stdlib.h>
#include <assert.h>

// Enum for determining if malloc calls were valid
typedef enum
{
    VALID_PTR = 1,
    INVALID_PTR = 0
} valid_ptr_t;

typedef enum
{
    FREE_NODES,
    NO_FREE_NODES
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
    dnode_t * node;
    int index;
    size_t length;
} dlist_iter_t;

static dnode_t * init_node(void * data);
static valid_ptr_t verify_alloc(void * ptr);
static void dlist_destroy_(dlist_t * dlist, dlist_settings_t delete, void(*free_func)(void *));



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
 * @brief Initialize the base linked list structure
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
 * @brief Append an item to the end of the linked list
 * @param dlist
 * @param data
 */
void dlist_append(dlist_t * dlist, void * data)
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

    // If tail is None, then we know that there is no items in the linked
    // list. So this item will be the very first item appended
    if (NULL == dlist->tail)
    {
        dlist->head = node;
        dlist->tail = node;
    }
    else
    {
        dlist->tail->next = node;
        node->prev = dlist->tail;
        dlist->tail = node;
    }
    dlist->length++;
}

/*!
 * @brief create an iter object to efficiently iterate over the linked list
 * of objects
 *
 * @param dlist
 * @return
 */
dlist_iter_t * dlist_get_iterable(dlist_t * dlist)
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
        .length     = dlist->length,
        .index      = -1,
        .node       = dlist->head
    };
    return iter;
}

/*!
 * @brief Iterates over the iterable and returns the next node. A NULL is
 * returned if the next node is NULL
 * @param dlist_iter
 * @return
 */
void * dlist_get_iter_next(dlist_iter_t * dlist_iter)
{
    if (NULL == dlist_iter->node)
    {
        return NULL;
    }
    dnode_t * node = dlist_iter->node;
    dlist_iter->node = dlist_iter->node->next;
    dlist_iter->index++;
    return node->data;
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

    // update the child parent node
    dlist->tail = node->prev;
    dlist->tail->next = NULL;
    dlist->length--;

    void * data = node->data;
    free(node);
    return data;
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

    // update the child parent node
    dlist->head = node->next;
    dlist->head->prev = NULL;
    dlist->length--;

    void * data = node->data;
    free(node);
    return data;
}

void * dlist_find_value(dlist_t * dlist, void * data)
{
    if (NULL == dlist->compare_func)
    {
        return NULL;
    }

    dlist_iter_t * iter = dlist_get_iterable(dlist);
    dnode_t * node;
    dlist_match_t found = DLIST_MISS_MATCH;
    while (NULL != (node = dlist_get_iter_next(iter)))
    {
        if (DLIST_MATCH == dlist->compare_func(node, data))
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
void * dlist_get_value(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    return dlist_find_value(dlist, data);
}

void * dlist_remove_value(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    dnode_t * node = dlist_find_value(dlist, data);
    if (NULL == node)
    {
        return NULL;
    }

    // preserve the node data before removing
    void * node_data = node->data;

    // check if node is the head, if so, update
    if (dlist->head == node)
    {
        dlist->head = node->next;
        if (NULL != dlist->head)
        {
            dlist->head->prev = NULL;
        }
    }
    if (dlist->tail == node)
    {
        dlist->tail = node->prev;
        if (NULL != dlist->tail)
        {
            dlist->tail->next = NULL;
        }
    }
    if (NULL != node->prev)
    {
        node->prev->next = node->next;
    }
    if (NULL != node->next)
    {
        node->next->prev = node->prev;
    }

    free(node);
    return node_data;
}

dlist_match_t dlist_in_dlist(dlist_t * dlist, void * data)
{
    assert(dlist);
    assert(data);
    dnode_t * node = dlist_find_value(dlist, data);
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

