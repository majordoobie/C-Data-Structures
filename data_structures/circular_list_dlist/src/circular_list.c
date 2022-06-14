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

    dlist_iter_t * iter = dlist_get_iterable(dlist, ITER_HEAD);
    if (NULL == iter)
    {
        fprintf(stderr, "[!] Unable to allocate memory for circular "
                        "linked list\n");
        dlist_destroy_iter(iter);
        dlist_destroy(dlist);
        free(clist);
        return NULL;
    }

    // Initialize the struct
    * clist = (clist_t) {
        .dlist          = dlist,
        .iter           = dlist_get_iterable(dlist, ITER_HEAD),
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

int32_t clist_get_length(clist_t * clist)
{
    return dlist_length(clist->dlist);
}

clist_result_t clist_insert(clist_t * clist, void * node, int32_t index, clist_location_t insert_at)
{
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
        dlist_insert(clist->dlist, node, index);
    }

}
