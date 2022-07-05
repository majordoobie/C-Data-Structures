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

dlist_iter_t * iter_get_iterable(dnode_t * node, dlist_t * dlist, int32_t index)
{
    // verify that the dlist is a valid pointer
    assert(dlist);
    dlist_iter_t * iter = (dlist_iter_t *)malloc(sizeof(dlist_iter_t));
    if (NULL == iter)
    {
        fprintf(stderr, "[!] Unable to allocate memory for iter object\n");
        return NULL;
    }

    *iter = (dlist_iter_t){
        .node       = node,
        .dlist      = dlist,
        .index      = index
    };

    return iter;
}
void iter_set_iter_node(dlist_iter_t * iter, dnode_t * node, int32_t index)
{
    assert(iter);
    assert(node);
    iter->node = node;
}

int32_t iter_get_iter_index(dlist_iter_t * iter)
{
    return iter->index;
}

dlist_t * iter_get_dlist(dlist_iter_t * iter)
{
    return iter->dlist;
}
