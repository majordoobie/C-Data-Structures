#ifndef BST_TEST_FUNCTIONS_H
#define BST_TEST_FUNCTIONS_H
#include "bst.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct bst_node_payload_t {
  int value;
  int other_value;
} my_structure;

typedef struct {
  my_structure **my_struct_list;
  int count;
} iter_struct_t;

typedef struct {
  int target;
  my_structure *payload;
} recursion_test_t;

bst_compare_t compare(my_structure *new_payload, my_structure *current_payload);
void free_payload(my_structure *node_payload);
my_structure *create_payload(int val1, int val2);

void print(my_structure *node_payload);
bst_recurse_t save_nodes(my_structure *node_payload, void *ptr);

#endif // BST_TEST_FUNCTIONS_H
