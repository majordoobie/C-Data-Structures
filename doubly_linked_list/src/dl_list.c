#include <stdio.h>
#include <dl_list.h>
#include <stdlib.h>

// Enum for determining if malloc calls were valid
typedef enum
{
    VALID_PTR = 1,
    INVALID_PTR = 0
} valid_ptr_t;

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
} dlist_t;

static valid_ptr_t verify_alloc(void * ptr);

dlist_t * dlist_init()
{
    dlist_t * dlist = (dlist_t *)calloc(1, sizeof(dlist_t));
    verify_alloc(dlist);
    return dlist;
}

//void * get_value(dlist_t * dlist, int32_t index)
//{
//    ;;
//}
//
/*!
 * @brief Free the double linked list without freeing the satellite data.
 *
 * This function will leave the satellite data up to the user to free.
 * @param dlist
 * @return
 */
void dlist_destroy(dlist_t * dlist)
{
    dnode_t * node = dlist->head;
    dnode_t * next_node;
    while (NULL != node)
    {
        next_node = node->next;
        free(node);
        node = next_node;
    }
    free(dlist);
}
void dlist_destroy_free(dlist_t * dlist, void (* free_func)(void *));

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
