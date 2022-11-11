#ifndef BST_H
#define BST_H

#include <stddef.h>
#include <stdint.h>

/*
 * bst_node_payload_t is the INTERNAL struct tag name. You must typedef the
 * expected struct tag with whatever alias you would like to use.
 *
 * i.e. in your code:
 * typedef struct bst_node_payload_t
 * {
 *      int key;
 *      void * data;
 * } your_alias_t;
 *
 * By creating two typedefs with the same tag, we can access both the internal
 * name and whatever name you would like to use
 *
 * Although there exists an internal name, bst has no idea what structure that
 * is. It could be a single value for all it cared. It is using the compare
 * function passed in to make the decisions.
 */
typedef struct bst_node_payload_t node_payload_t;

typedef enum {
  BST_INSERT_SUCCESS,
  BST_INSERT_FAILURE,
  BST_REMOVE_SUCCESS,
  BST_REMOVE_FAILURE,
  BST_NODE_NOT_FOUND,
} bst_status_t;

typedef enum {
  REPLACE_PAYLOAD_TRUE,
  REPLACE_PAYLOAD_FALSE,
} bst_replace_t;

typedef enum {
  FREE_PAYLOAD_TRUE,
  FREE_PAYLOAD_FALSE,
} bst_destroy_t;

typedef enum {
  BST_LT,
  BST_GT,
  BST_EQ,
} bst_compare_t;

typedef enum {
  TRAVERSAL_IN_ORDER,
  TRAVERSAL_POST_ORDER,
  TRAVERSAL_PRE_ORDER,
} bst_traversal_t;

/*
 * Values are used during traversal of the nodes. They can be used by
 * external callback functions to instruct the recursion functions to stop
 * what they are doing
 */
typedef enum {
  RECURSE_TRUE,
  RECURSE_STOP,
} bst_recurse_t;

/*
 * The entire tree is stored in the avl_tree_t structure along with the passed
 * in functions from the user.
 */
typedef struct node node_t;
typedef struct bst {
  node_t *root;
  bst_compare_t (*compare)(node_payload_t *, node_payload_t *);
  void (*free_payload)(node_payload_t *);
} avl_tree_t;

avl_tree_t *bst_init(bst_compare_t (*compare)(node_payload_t *,
                                              node_payload_t *),
                     void (*free_payload)(node_payload_t *));
bst_status_t
bst_insert(avl_tree_t *bst, node_payload_t *payload, bst_replace_t replace,
           bst_status_t (*callback)(node_payload_t *, void *, void *),
           void *ptr);
bst_status_t bst_remove(avl_tree_t *bst, node_payload_t *payload,
                        bst_destroy_t free_payload);
void bst_destroy(avl_tree_t *bst, bst_destroy_t free_payload);
void bst_traversal(avl_tree_t *bst, bst_traversal_t type,
                   bst_recurse_t (*callback)(node_payload_t *, void *),
                   void *void_ptr);
node_payload_t *bst_get_node(avl_tree_t *bst, node_payload_t *payload);
void print_2d(avl_tree_t *bst, void (*callback)(node_payload_t *));

#endif // BST_H
