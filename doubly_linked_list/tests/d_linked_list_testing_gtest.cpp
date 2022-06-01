#include <gtest/gtest.h>
#include <dl_list.h>


// basic init test
TEST(dlist_test, InitTest)
{
    dlist_t * dlist = dlist_init();
    ASSERT_NE(dlist, nullptr);
    dlist_destroy(dlist);
}