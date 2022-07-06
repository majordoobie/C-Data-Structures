#include <dl_iter.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct dlist_iter_t
{
    dlist_t * dlist;
    dnode_t * node;
    int32_t index;
} dlist_iter_t;

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

void iter_destroy_iterable(dlist_iter_t * iter)
{
    free(iter);
}



void iter_set_iter_node(dlist_iter_t * iter, dnode_t * node, int32_t index)
{
    assert(iter);
    assert(node);
    iter->node = node;
    iter->index = index;
}

int32_t iter_get_iter_index(dlist_iter_t * iter)
{
    return iter->index;
}

dlist_t * iter_get_dlist(dlist_iter_t * iter)
{
    return iter->dlist;
}

dnode_t * iter_get_iter_node(dlist_iter_t * iter)
{
    return iter->node;
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