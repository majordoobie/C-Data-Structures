#include <stdio.h>
#include <stdlib.h>
#include <tree_map_avl.h>

typedef struct dict_t {
  avl_tree_t *tree;
  bst_recurse_t (*value_search)(node_payload_t *, void *);
  int count;
} dict_t;

/*!
 * @brief Create a map between a key-value pair using a AVL tree as the
 * backbone of the data structure
 *
 * The dict_t structure piggybacks off the avl_bst_t structure therefore it
 * requires mostly the same inputs for initialization. It requires a compare
 * function used to decide where the nodes are going to be placed in the tree
 * structure. It will then require a function to free the individual nodes
 * and for this implementation it requires a function to compare the values.
 *
 * The last function is unique to this data structure. It allows the user to
 * search linearly for a value with in the data structure. This is not a fast
 * implementation and should be used sparingly. l
 *
 * @param compare Function to compare nodes and create the tree structure
 * @param free_payload Function to free the nodes
 * @param value_search Function to search for a value instead of a key par
 * @return
 */
dict_t *init_dict(bst_compare_t (*compare)(key_val_t *, key_val_t *),
                  void (*free_payload)(node_payload_t *),
                  bst_recurse_t (*value_search)(node_payload_t *, void *)) {
  dict_t *dict = calloc(1, sizeof(*dict));
  if (NULL == dict) {
    fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(*dict));
    abort();
  }
  dict->tree = bst_init(compare, free_payload);
  if (NULL == dict->tree) {
    fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n",
            sizeof(avl_tree_t));
    abort();
  }
  dict->value_search = value_search;
  return dict;
}

/*!
 * @brief Destroys the dictionary object. This is dependent on the
 * "free_payload" callback function provided.
 * @param dict
 */
void destroy_dict(dict_t *dict) {
  bst_destroy(dict->tree, FREE_PAYLOAD_TRUE);
  free(dict);
}

/*!
 * @brief Insert a key_val_t into the dictionary. If successful, return a 1
 * otherwise 0.
 * @param dict[in] dict_t
 * @param key_val[in] key_val_t
 * @return 1 for successful insertion or 0 otherwise
 */
uint8_t put_key_val(dict_t *dict, key_val_t *key_val) {
  bst_status_t status =
      bst_insert(dict->tree, key_val, REPLACE_PAYLOAD_TRUE, NULL, NULL);
  if (status == BST_INSERT_SUCCESS) {
    dict->count++;
    return 1;
  } else {
    return 0;
  }
}

/*!
 * @brief If more granular control is needed for how to handle when nodes are
 * found in the tree you have the option of supplying a callback function for
 * the node and also providing a void pointer to whatever structure is needed to
 * be successful.
 * @param dict
 * @param key_val
 * @param callback
 * @param ptr
 * @return
 */
uint8_t put_key_val_adv(dict_t *dict, key_val_t *key_val,
                        bst_status_t (*callback)(key_val_t *key_val, void *,
                                                 void *),
                        void *ptr) {
  bst_status_t status =
      bst_insert(dict->tree, key_val, REPLACE_PAYLOAD_TRUE, callback, ptr);
  if (status == BST_INSERT_SUCCESS) {
    dict->count++;
    return 1;
  } else {
    return 0;
  }
}

/*!
 * @brief Get a key_val_t payload from the dictionary.
 * @param dict[in] dict_t
 * @param key[in] key_val_t
 * @return Returns the key_val_t or NULL if not found
 */
key_val_t *get_key_val(dict_t *dict, key_val_t *key) {
  return bst_get_node(dict->tree, key);
}

/*!
 * @brief Check if the collation_value passed in is found in the dictionary
 * @param dict[in] dict_t
 * @param key[in] key_val_t
 * @return Returns true if collation_value is found else return false
 */
bool contains_key(dict_t *dict, key_val_t *key) {
  key_val_t *result = bst_get_node(dict->tree, key);
  if (NULL == result) {
    return false;
  }
  return true;
}

/*!
 * @brief Function calls upon the traversal function of the backend AVL tree and
 * process each node with the given "value_search" function. Recursion stops
 * when the provided functions instructs it to stop. Nothing is returned. For
 * this reason, it is wise to use the void ptr for returning back any values you
 * may wish.
 * @param dict[in] dict_t
 * @param ptr void ptr to what ever object you may need for this function.
 */
void contains_value(dict_t *dict, void *ptr) {
  bst_traversal(dict->tree, TRAVERSAL_PRE_ORDER, dict->value_search, ptr);
}

/*!
 * @brief Remove a collation_value value pair from the dictionary
 * @param dict[in] dict_t
 * @param key[in] key_val_t
 */
void remove_key_val(dict_t *dict, key_val_t *key) {
  if (BST_REMOVE_SUCCESS == bst_remove(dict->tree, key, FREE_PAYLOAD_TRUE)) {
    dict->count--;
  }
}

void recurse_dict(dict_t *dict,
                  bst_recurse_t (*callback)(key_val_t *key_val, void *ptr),
                  void *ptr) {
  bst_traversal(dict->tree, TRAVERSAL_IN_ORDER, callback, ptr);
}

int get_size(dict_t *dict) { return dict->count; }
