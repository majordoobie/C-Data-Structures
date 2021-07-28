#include <tree_map_avl.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct dict_t
{
    bst_t * tree;
    int count;
} dict_t;

/*!
 * @brief Initializes a dictionary "tree map"
 * @param compare[in] Compare function used to compare two keys to determine order
 * @param free_payload[in] Callback function used to free the key_val_t
 * @return Dictionary object or abort
 */
dict_t * init_dict(bst_compare_t (* compare)(key_val_t *, key_val_t *), void (* free_payload)(node_payload_t *))
{
    dict_t * dict = calloc(1, sizeof(* dict));
    if (NULL == dict)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(* dict));
        abort();
    }
    dict->tree = bst_init(compare, free_payload);
    if (NULL == dict->tree)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(bst_t));
        abort();
    }
    return dict;
}

/*!
 * @brief Destroys the dictionary object. This is dependent on the "free_payload" callback
 * function provided.
 * @param dict
 */
void destroy_dict(dict_t * dict)
{
    bst_destroy(dict->tree, FREE_PAYLOAD_TRUE);
    free(dict);
}
