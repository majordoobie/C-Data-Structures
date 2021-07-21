#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H
#include <stdio.h>
#include <bst.h>
#include <stdlib.h>

typedef struct external_t {
    int value;
    int other_value;

} my_structure;

bst_compare_t compare(my_structure * new_payload, my_structure * current_payload);
void print(my_structure * node_payload);
void free_payload(my_structure * node_payload);
my_structure * create_payload(int val1, int val2);

#endif //TEST_FUNCTIONS_H
