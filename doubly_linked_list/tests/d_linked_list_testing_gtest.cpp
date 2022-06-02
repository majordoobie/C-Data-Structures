#include <gtest/gtest.h>
#include <dl_list.h>


/*
 * Helper Functions for testing
 */
// Creates a simple payload for the linked list
char * get_payload(int num)
{
    const int size = 20;
    char buff[size];
    snprintf(buff, size, "Hello world: %d\n", num);
    return strdup(buff);
}

// frees the payload inserted into the linked list
void free_payload(void * data)
{
    free(data);
}






/*
 * //end of Helper Functions for testing
 */

// basic init test
TEST(dlist_test, InitTest)
{
    dlist_t * dlist = dlist_init();
    ASSERT_NE(dlist, nullptr);
    dlist_destroy(dlist);
}

TEST(dlist_test, InsertTest)
{
    dlist_t * dlist = dlist_init();
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));

    dlist_destroy_free(dlist, free_payload);
}
