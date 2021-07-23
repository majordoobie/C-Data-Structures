#include <bst_test_functions.h>

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
    printf("%d", node_payload->value);
}

// Save nodes into a list
void save_nodes(my_structure * node_payload, void * ptr)
{
    iter_struct_t * iter = (iter_struct_t *) ptr;
    iter->my_struct_list[iter->count] = node_payload;
    iter->count++;
}

void free_payload(my_structure * node_payload)
{
    free(node_payload);
}


my_structure * create_payload(int val1, int val2)
{
    my_structure * data = calloc(1, sizeof(* data));
    data->value = val1;
    data->other_value = val2;
    return data;
}

