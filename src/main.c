#include <stdio.h>
#include <bst.h>
#include <stdlib.h>


bst_compare_t compare(my_structure * new_payload, my_structure * current_payload)
{
    if (new_payload->value < current_payload->value)
    {
        return BST_LT;
    }
    else if (new_payload->value > current_payload->value)
    {
        return BST_GT;
    }
    else
    {
        return BST_EQ;
    }
}

void print(my_structure * node_payload)
{
    printf("[%d | %d] ", node_payload->value, node_payload->other_value);
}

void free_my(my_structure * node_payload)
{
    free(node_payload);
}

int main(void)
{
    my_structure * data = calloc(1, sizeof(* data));
    my_structure * data2 = calloc(1, sizeof(* data));
    my_structure * data3 = calloc(1, sizeof(* data));
    data->value = 1000;
    data->other_value = 20;

    data2->value = 39;
    data2->other_value = 50;

    data3->value = 1;
    data3->other_value = 99;

    // initialize the BST structure
    bst_t * tree = init_bst(compare, free_my);
    // insert the first node which will be the root node
    insert_node(tree, data, BST_REPLACE_TRUE);
    insert_node(tree, data2, BST_REPLACE_TRUE);
    insert_node(tree, data3, BST_REPLACE_TRUE);

    traversal_bst(tree, BST_IN_ORDER, print);


    my_structure * data4 = calloc(1, sizeof(* data));
    data4->value = 39;
    data4->other_value = 999999;
    printf("\n=> %d\n", insert_node(tree, data4, BST_REPLACE_TRUE));

    printf("\n");
    traversal_bst(tree, BST_IN_ORDER, print);
    printf("\n");
    traversal_bst(tree, BST_PRE_ORDER, print);
    destroy_bst(tree, BST_FREE_PAYLOAD_TRUE);
}

