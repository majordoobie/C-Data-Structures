#ifndef TREE_MAP_AVL_H
#define TREE_MAP_AVL_H
#include <bst.h>
#include <stdbool.h>

typedef struct bst_node_payload_t key_val_t;
typedef struct dict_t dict_t;

dict_t *init_dict(bst_compare_t (*compare)(key_val_t *, key_val_t *),
                  void (*free_payload)(node_payload_t *),
                  bst_recurse_t (*value_search)(key_val_t *, void *));
void destroy_dict(dict_t *dict);
uint8_t put_key_val(dict_t *dict, key_val_t *key_val);
uint8_t put_key_val_adv(dict_t *dict, key_val_t *key_val,
                        bst_status_t (*callback)(key_val_t *key_val, void *,
                                                 void *),
                        void *ptr);
key_val_t *get_key_val(dict_t *dict, key_val_t *key);
bool contains_key(dict_t *dict, key_val_t *key);
void contains_value(dict_t *dict, void *ptr);
void remove_key_val(dict_t *dict, key_val_t *key);
int get_size(dict_t *dict);

void recurse_dict(dict_t *dict,
                  bst_recurse_t (*callback)(key_val_t *key_val, void *ptr),
                  void *ptr);

#endif // TREE_MAP_AVL_H
