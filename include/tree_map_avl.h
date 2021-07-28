#ifndef TREE_MAP_AVL_H
#define TREE_MAP_AVL_H
#include <bst.h>

typedef struct bst_node_payload_t key_val_t;
typedef struct dict_t dict_t;


dict_t * init_dict(bst_compare_t (* compare)(key_val_t *, key_val_t *), void (* free_payload)(node_payload_t *));
void destroy_dict(dict_t * dict);


#endif //TREE_MAP_AVL_H
