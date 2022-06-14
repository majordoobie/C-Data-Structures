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

clist_t * clist_destroy(clist_t * clist, clist_delete_t remove_nodes);
