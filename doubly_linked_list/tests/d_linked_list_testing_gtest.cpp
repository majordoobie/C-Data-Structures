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

TEST(dlist_test, IterableTest)
{
    dlist_t * dlist = dlist_init();
    ASSERT_NE(dlist, nullptr);
    dlist_append(dlist, get_payload(1));
    dlist_append(dlist, get_payload(2));
    dlist_append(dlist, get_payload(3));

    dlist_iter_t * iter = dlist_get_iterable(dlist);
    char * node = (char*)dlist_get_iter_next(iter);
    while (nullptr != node)
    {
        printf("%s", node);
        node = (char*)dlist_get_iter_next(iter);

    }
//    while (!(dlist_iter_is_empty(iter)))
//    {
//        printf("%s", (char*)dlist_get_iter_next(iter));
//    }

    dlist_destroy_iter(iter);
    dlist_destroy_free(dlist, free_payload);

}
