#include <circular_list.h>
#include <dl_list.h>
#include <stdio.h>
#include <malloc.h>

typedef struct clist_t
{
    dlist_t * dlist;
    dlist_iter_t * iter;
    uint32_t clist_size;
    clist_match_t (* compare_func)(void *, void *);
    void (* free_func)(void *);
} clist_t;

/*!
 * @brief Initialize the circular linked list with a size limit. If any errors
 * occur then a NULL is returned otherwise a pointer to the clist is retured.
 * @param list_size
 * @param compare_func
 * @param free_func
 * @return NULL or error or clist_t pointer
 */
clist_t * clist_init(uint32_t list_size, clist_match_t (* compare_func)(void *, void *), void (* free_func)(void *))
{
    dlist_t * dlist = dlist_init((dlist_match_t (*)(void *, void *))compare_func);

    // Dlist will already print an error message
    if (NULL == dlist)
    {
        return NULL;
    }

    clist_t * clist = (clist_t *)malloc(sizeof(clist_t));
    if (NULL == clist)
    {
        fprintf(stderr, "[!] Unable to allocate memory for circular "
                        "linked list\n");
        dlist_destroy(dlist);
    }

    dlist_iter_t * iter = (dlist_iter_t *)dlist_get_iterable(dlist, ITER_HEAD);
    if (NULL == iter)
    {
        fprintf(stderr, "[!] Unable to allocate memory for circular "
                        "linked list\n");
        dlist_destroy(dlist);
    }


    // Initialize the struct
    * clist = (clist_t) {
        .dlist          = dlist,
        .iter           = iter,
        .clist_size     = list_size,
        .compare_func   = compare_func,
        .free_func      = free_func,
    };

    return clist;
}

/*!
 * @brief Free the circular linked list with the option of freeing all the nodes
 * using the free function pointer.
 * @param clist
 * @param remove_nodes
 * @return
 */
void clist_destroy(clist_t * clist, clist_delete_t remove_nodes)
{
    dlist_destroy_iter(clist->iter);
    if (FREE_NODES_TRUE == remove_nodes)
    {
        dlist_destroy_free(clist->dlist, clist->free_func);
    }
    else
    {
        dlist_destroy(clist->dlist);
    }

    free(clist);
}


/*!
 * @brief Return the number of elements in the list
 * @param clist
 * @return Number of elements
 */
size_t clist_get_length(clist_t * clist)
{
    return dlist_get_length(clist->dlist);
}

/*!
 * @brief Insert node at the given index. The new node will take on the given
 * index by moving the nodes already at the index to the right.
 *
 * If using clist_location_t of HEAD or TAIL the index parameter is ignored and
 * the node is inserted at the head or tail respectably. If using the the INDEX
 * clist_location_t then the index parameter is used as the location where the
 * new node is inserted. If the index is an invalid location a C_FAIL is
 * returned and the node is not inserted.
 *
 * @param clist
 * @param node
 * @param index
 * @param insert_at
 * @return C_SUCCESS if successful insert or C_FAIL
 */
clist_result_t clist_insert(clist_t * clist, void * node, int32_t index, clist_location_t insert_at)
{
    clist_result_t result = C_SUCCESS;
    if (HEAD == insert_at)
    {
        dlist_prepend(clist->dlist, node);
    }
    else if (TAIL == insert_at)
    {
        dlist_append(clist->dlist, node);
    }
    else
    {
        result = (clist_result_t)dlist_insert(clist->dlist, node, index);
        if (C_FAIL == result)
        {
            return result;
        }
    }

    // if this is the first item in the list then initialize the iter
    if (1 == dlist_get_length(clist->dlist))
    {
        dlist_set_iter_head(clist->iter);
    }
    return result;
}

/*!
 * @brief Return the value of the current node in the rotation of indexes
 * @param clist
 * @return Pointer to the current node or NULL if list is empty
 */
void * clist_get_value(clist_t * clist)
{
    return iter_get_value(clist->iter);
}

/*!
 * @brief Rotates the nodes to the right and returns the next node in the
 * rotation. If the previous node is the tail then the next node will be the
 * head of the linked list.
 * @param clist
 * @return Next node pointer or NULL if empty
 */
void * clist_get_next(clist_t * clist)
{
    void * node = dlist_get_iter_next(clist->iter);
    if (NULL == node)
    {
        dlist_set_iter_head(clist->iter);
        return iter_get_value(clist->iter);
    }
    return node;
}

/*!
 * @brief Fetch the node in the circular linked list by matching with the
 * node passed in. The search uses the comparison function passed in the
 * find the correct node. This does not remove the node from the list, you
 * must use the remove function for that. The remove function does both finding
 * and removing.
 * @param clist
 * @param node
 * @return Pointer to node if found else NULL
 */
void * clist_find(clist_t * clist, void * node)
{
    return dlist_get_by_value(clist->dlist, node);
}

/*!
 * @brief Remove a node from the linked list. If the node is not found a NULL
 * is returned. Otherwise, the node is returned and it is up to the caller to
 * free the node.
 * @param clist
 * @param node
 * @return Pointer to node or NULL if not found
 */
void * clist_remove(clist_t * clist, void * node)
{
    // get the index of the node in the iter if any to understand if we need
    // to manipulate the iter object
    void * data = dlist_remove_value(clist->dlist, node);

    if (NULL == data)
    {
        return data;
    }



    return data;
}

void clist_quick_sort(clist_t * clist,
                      sort_order_t order,
                      clist_compare_t (* compare_func)(void *, void *))
{
    dlist_quick_sort(clist->dlist, (sort_direction_t)order,
                     (dlist_compare_t (*)(void *, void *))compare_func);
}
