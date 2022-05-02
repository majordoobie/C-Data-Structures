#ifndef INTERSECT_H
#define INTERSECT_H
#include "tree_map_avl.h"

typedef struct bst_node_payload_t
{
    uint32_t file_count;
    wchar_t * collation_value;     // utf-8 string is transform to follow LC_COLLATION for faster comparisons
    wchar_t * original;            // original string
    dict_t * variations;           // Uses dict specified int variation.h
} key_val_t;

typedef struct
{
    dict_t * dict;
    int file_iteration;
} intersect_t;

intersect_t * create_intersect_dict();

bst_recurse_t reduce_search(key_val_t * node_payload, void * ptr);
bst_recurse_t intersect_print(key_val_t * payload, void * ptr);

bst_status_t intersect_adv_add(key_val_t * payload, void * ptr, void * ptr2);
key_val_t * create_intersect_payload(const wchar_t * word, uint32_t file_count);
bst_compare_t intersect_dict_compare(key_val_t * current_payload, key_val_t * new_payload);
void intersect_free_payload(key_val_t * payload);

void variation_free_payload(key_val_t * payload);
key_val_t * variation_create(const wchar_t * word);
bst_recurse_t variation_print(key_val_t * payload, void * ptr);

void intersect_reduce(dict_t * dict, int interation);
#endif //INTERSECT_H
