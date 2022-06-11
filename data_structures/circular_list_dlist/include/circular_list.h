#ifndef DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_
#define DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_

#ifdef __cplusplus
extern "C"{
#endif
    /*
     *  Creating a circularly linked list with n number of items
 Navigating through a circularly linked list
 Finding the first occurrence of an item in a circularly linked list
 Sorting the circularly linked list alphanumerically using a function pointer
 Removing selected items from the circularly linked list
 Inserting an item into a specific location in a circularly linked list
 Removing all items from the circularly linked list
 Destroying a circularly linked list
     */

#include <stdint.h>
typedef struct clist_t clist_t;
clist_t * clist_init(uint32_t list_size);


#ifdef __cplusplus
};
#endif
#endif //DATA_STRUCTURES_C_CIRCULAR_LIST_DLIST_SRC_CIRCULAR_LIST_H_
