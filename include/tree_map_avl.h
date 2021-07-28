#ifndef TREE_MAP_AVL_H
#define TREE_MAP_AVL_H
#include <bst.h>

typedef struct bst_node_payload_t
{
    int value;
    int other_value;
} key_val_t;

typedef struct
{
    bst_t * tree;
    int count;
} dict_t;

dict_t * init_dict(bst_compare_t (* compare)(node_payload_t *, node_payload_t *), void (* free_payload)(node_payload_t *))
void destroy_dict(dict_t * dict);


#endif //TREE_MAP_AVL_H
